#include "ChowCentaurPlugin.h"
#include "CustomLNFs.h"

namespace
{
const String gainTag = "gain";
const String trebleTag = "treble";
const String levelTag = "level";
const String neuralTag = "neural";
const String bypassTag = "bypass";
const String monoTag = "mono";
} // namespace

ChowCentaur::ChowCentaur() : gainStageProc (vts),
                             gainStageMLProc (vts)
{
    trebleParam = vts.getRawParameterValue (trebleTag);
    levelParam = vts.getRawParameterValue (levelTag);
    mlParam = vts.getRawParameterValue (neuralTag);
    bypassParam = vts.getRawParameterValue (bypassTag);
    monoParam = vts.getRawParameterValue (monoTag);

    LookAndFeel::setDefaultLookAndFeel (&myLNF);
    scope = magicState.createAndAddObject<foleys::MagicOscilloscope> ("scope");
}

ChowCentaur::~ChowCentaur()
{
}

void ChowCentaur::addParameters (Parameters& params)
{
    params.push_back (std::make_unique<AudioParameterFloat> (gainTag, "Gain", 0.0f, 1.0f, 0.5f));
    params.push_back (std::make_unique<AudioParameterFloat> (trebleTag, "Treble", 0.0f, 1.0f, 0.5f));
    params.push_back (std::make_unique<AudioParameterFloat> (levelTag, "Level", 0.0f, 1.0f, 0.5f));
    params.push_back (std::make_unique<AudioParameterChoice> (neuralTag, "Mode", StringArray { "Traditional", "Neural" }, 0));
    params.push_back (std::make_unique<AudioParameterBool> (bypassTag, "Bypass", false));
    params.push_back (std::make_unique<AudioParameterBool> (monoTag, "Mono", false));
}

void ChowCentaur::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    gainStageProc.reset (sampleRate, samplesPerBlock);
    gainStageMLProc.reset (sampleRate, samplesPerBlock);

    for (int ch = 0; ch < 2; ++ch)
    {
        inProc[ch].reset ((float) sampleRate);
        tone[ch].reset ((float) sampleRate);
        outProc[ch].reset ((float) sampleRate);
    }

    scope->prepareToPlay (sampleRate, samplesPerBlock);

    useMLPrev = static_cast<bool> (*mlParam);
    fadeBuffer.setSize (getMainBusNumOutputChannels(), samplesPerBlock);

    useMonoPrev = static_cast<bool> (*monoParam);
    monoBuffer.setSize (1, samplesPerBlock);

    // set up DC blockers
    *dcBlocker.state = *dsp::IIR::Coefficients<float>::makeHighPass (sampleRate, 35.0f);
    dsp::ProcessSpec spec { sampleRate, static_cast<uint32> (samplesPerBlock), 2 };
    dcBlocker.prepare (spec);

    bypass.prepare (samplesPerBlock, ! bypass.toBool (bypassParam));
}

void ChowCentaur::releaseResources()
{
}

void ChowCentaur::processInternalBuffer (AudioBuffer<float>& buffer)
{
    const auto numSamples = buffer.getNumSamples();
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        auto* x = buffer.getWritePointer (ch);

        // Input buffer
        FloatVectorOperations::multiply (x, 0.5f, numSamples);
        inProc[ch].processBlock (x, numSamples);
        FloatVectorOperations::clip (x, x, -4.5f, 4.5f, numSamples); // op amp clipping
    }

    const bool useML = static_cast<bool> (*mlParam);
    if (useML == useMLPrev)
    {
        if (useML) // use rnn
            gainStageMLProc.processBlock (buffer);
        else // use circuit model
            gainStageProc.processBlock (buffer);
    }
    else
    {
        fadeBuffer.makeCopyOf (buffer, true);

        if (useML) // use rnn
        {
            gainStageMLProc.processBlock (buffer);
            gainStageProc.processBlock (fadeBuffer);
        }
        else // use circuit model
        {
            gainStageProc.processBlock (buffer);
            gainStageMLProc.processBlock (fadeBuffer);
        }

        buffer.applyGainRamp (0, numSamples, 0.0f, 1.0f);
        for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
            buffer.addFromWithRamp (ch, 0, fadeBuffer.getReadPointer (ch), numSamples, 1.0f, 0.0f);

        useMLPrev = useML;
    }

    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        auto* x = buffer.getWritePointer (ch);

        // tone stage
        tone[ch].setTreble (*trebleParam);
        tone[ch].processBlock (x, numSamples);

        FloatVectorOperations::multiply (x, -1.0f, numSamples); // inverting amplifier
        FloatVectorOperations::clip (x, x, -13.1f, 11.7f, numSamples); // op-amp clipping

        outProc[ch].setLevel (*levelParam);
        outProc[ch].processBlock (x, numSamples);
    }
}

void ChowCentaur::processAudioBlock (AudioBuffer<float>& buffer)
{
    ScopedNoDenormals noDenormals;

    // bypass if needed
    if (! bypass.processBlockIn (buffer, ! bypass.toBool (bypassParam)))
    {
        // DC Blocker
        dsp::AudioBlock<float> block (buffer);
        dsp::ProcessContextReplacing<float> context (block);
        dcBlocker.process (context);

        scope->pushSamples (buffer);

        return;
    }

    // process mono or stereo buffer?
    auto numSamples = buffer.getNumSamples();
    auto useMono = static_cast<bool> (monoParam->load());

    if (useMono != useMonoPrev)
    {
        gainStageProc.reset (getSampleRate(), getBlockSize());
        gainStageMLProc.reset (getSampleRate(), getBlockSize());

        for (int ch = 0; ch < 2; ++ch)
        {
            inProc[ch].reset ((float) getSampleRate());
            tone[ch].reset ((float) getSampleRate());
            outProc[ch].reset ((float) getSampleRate());
        }

        useMonoPrev = useMono;
    }

    if (useMono)
    {
        monoBuffer.setSize (1, buffer.getNumSamples(), false, false, true);
        monoBuffer.clear();
        monoBuffer.copyFrom (0, 0, buffer.getReadPointer (0), numSamples);

        for (int ch = 1; ch < buffer.getNumChannels(); ++ch)
            monoBuffer.addFrom (0, 0, buffer.getReadPointer (ch), numSamples);

        monoBuffer.applyGain (1.0f / (float) buffer.getNumChannels());
    }


    auto& processBuffer = useMono ? monoBuffer : buffer;

    // actual DSP processing
    processInternalBuffer (processBuffer);

    // go back from mono to stereo (if needed)
    if (useMono)
    {
        auto processedData = processBuffer.getReadPointer (0);
        for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
            buffer.copyFrom (ch, 0, processedData, numSamples);
    }

    // bypass fade
    bypass.processBlockOut (buffer, ! bypass.toBool (bypassParam));

    // DC Blocker
    dsp::AudioBlock<float> block (buffer);
    dsp::ProcessContextReplacing<float> context (block);
    dcBlocker.process (context);

    scope->pushSamples (buffer);
}

AudioProcessorEditor* ChowCentaur::createEditor()
{
    auto builder = chowdsp::createGUIBuilder (magicState);
    builder->registerLookAndFeel ("ComboBoxLNF", std::make_unique<ComboBoxLNF>());
    builder->registerLookAndFeel ("ButtonLNF", std::make_unique<ButtonLNF>());

    return new foleys::MagicPluginEditor (magicState, BinaryData::gui_xml, BinaryData::gui_xmlSize, std::move (builder));
}

void ChowCentaur::setStateInformation (const void* data, int sizeInBytes)
{
#if JUCE_IOS
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName (vts.state.getType()))
            vts.replaceState (juce::ValueTree::fromXml (*xmlState));
#else
    MessageManagerLock mml;
    magicState.setStateInformation (data, sizeInBytes, getActiveEditor());
#endif
}

void ChowCentaur::getStateInformation (MemoryBlock& data)
{
#if JUCE_IOS
    auto state = vts.copyState();
    std::unique_ptr<XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, data);
#else
    magicState.getStateInformation (data);
#endif
}

// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ChowCentaur();
}
