#include "ChowCentaurPlugin.h"
#include "CustomLNFs.h"

namespace
{
const String gainTag = "gain";
const String trebleTag = "treble";
const String levelTag = "level";
const String neuralTag = "neural";
const String bypassTag = "bypass";
} // namespace

ChowCentaur::ChowCentaur() : gainStageProc (vts),
                             gainStageMLProc (vts)
{
    trebleParam = vts.getRawParameterValue (trebleTag);
    levelParam = vts.getRawParameterValue (levelTag);
    mlParam = vts.getRawParameterValue (neuralTag);
    bypassParam = vts.getRawParameterValue (bypassTag);

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

    // set up DC blockers
    *dcBlocker.state = *dsp::IIR::Coefficients<float>::makeHighPass (sampleRate, 35.0f);
    dsp::ProcessSpec spec { sampleRate, static_cast<uint32> (samplesPerBlock), 2 };
    dcBlocker.prepare (spec);

    bypass.prepare (samplesPerBlock, ! bypass.toBool (bypassParam));
}

void ChowCentaur::releaseResources()
{
}

void ChowCentaur::processAudioBlock (AudioBuffer<float>& buffer)
{
    ScopedNoDenormals noDenormals;

    if (! bypass.processBlockIn (buffer, ! bypass.toBool (bypassParam)))
    {
        // DC Blocker
        dsp::AudioBlock<float> block (buffer);
        dsp::ProcessContextReplacing<float> context (block);
        dcBlocker.process (context);

        scope->pushSamples (buffer);

        return;
    }

    auto numSamples = buffer.getNumSamples();
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
    MessageManagerLock mml;
    magicState.setStateInformation (data, sizeInBytes, getActiveEditor());
}

// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ChowCentaur();
}
