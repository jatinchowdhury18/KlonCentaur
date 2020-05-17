#include "ChowCentaurPlugin.h"

namespace
{
    const String gainTag   = "gain";
    const String trebleTag = "treble";
    const String levelTag  = "level";
}

ChowCentaur::ChowCentaur()
{
    gainParam   = vts.getRawParameterValue (gainTag);
    trebleParam = vts.getRawParameterValue (trebleTag);
    levelParam  = vts.getRawParameterValue (levelTag);

    scope = magicState.addPlotSource ("scope", std::make_unique<foleys::MagicOscilloscope>());
}

ChowCentaur::~ChowCentaur()
{
}
    
void ChowCentaur::addParameters (Parameters& params)
{
    params.push_back (std::make_unique<AudioParameterFloat> (gainTag,   "Gain",   0.0f, 1.0f, 0.5f));
    params.push_back (std::make_unique<AudioParameterFloat> (trebleTag, "Treble", 0.0f, 1.0f, 0.5f));
    params.push_back (std::make_unique<AudioParameterFloat> (levelTag,  "Level",  0.0f, 1.0f, 0.5f));
}

void ChowCentaur::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    os.initProcessing (samplesPerBlock);

    const auto osFactor = (int) os.getOversamplingFactor();
    for (int ch = 0; ch < 2; ++ch)
    {
        inProc[ch].reset ((float) sampleRate * osFactor);
        preAmp[ch].reset (sampleRate * osFactor);
        amp[ch].reset ((float) sampleRate * osFactor);
        clip[ch].reset (sampleRate * osFactor);
        ff2[ch].reset (sampleRate * osFactor);
        sumAmp[ch].reset ((float) sampleRate * osFactor);
        tone[ch].reset ((float) sampleRate * osFactor);
        outProc[ch].reset ((float) sampleRate* osFactor);
    }

    ff1Buff.setSize (2, samplesPerBlock * osFactor);
    ff2Buff.setSize (2, samplesPerBlock * osFactor);
    scope->prepareToPlay (sampleRate, samplesPerBlock);
}

void ChowCentaur::releaseResources()
{
    os.reset();
}

void ChowCentaur::processBlock (AudioBuffer<float>& buffer)
{
    ScopedNoDenormals noDenormals;

    dsp::AudioBlock<float> block (buffer);
    dsp::AudioBlock<float> osBlock (buffer);

    // upsample
    osBlock = os.processSamplesUp (block);
    const auto numSamples = (int) osBlock.getNumSamples();
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        auto* x = osBlock.getChannelPointer (ch);
        auto* x1 = ff1Buff.getWritePointer (ch);
        auto* x2 = ff2Buff.getWritePointer (ch);

        // Input buffer
        FloatVectorOperations::multiply (x, 0.5f, numSamples);
        inProc[ch].processBlock (x, numSamples);

        FloatVectorOperations::clip (x, x, 0.0f, 9.0f, numSamples);

        // side chain buffers
        FloatVectorOperations::copy (x2, x, numSamples);

        // Gain stage
        preAmp[ch].setGain (*gainParam);
        for (int n = 0; n < numSamples; ++n)
        {
            x[n] = preAmp[ch].processSample (x[n]);
            x1[n] = preAmp[ch].getFF1();
        }

        amp[ch].setGain (*gainParam);
        amp[ch].processBlock (x, numSamples);
        FloatVectorOperations::clip (x, x, 0.0f, 9.0f, numSamples);

        // clipping stage
        for (int n = 0; n < numSamples; ++n)
            x[n] = clip[ch].processSample (x[n]);
        
        // Feed forward network 2
        ff2[ch].setGain (*gainParam);
        for (int n = 0; n < numSamples; ++n)
            x2[n] = ff2[ch].processSample (x2[n]);

        // summing amp
        FloatVectorOperations::add (x, x1, numSamples);
        FloatVectorOperations::add (x, x2, numSamples);
        sumAmp[ch].processBlock (x, numSamples);
        FloatVectorOperations::clip (x, x, -8.6f, 16.2f, numSamples);

        // tone stage
        tone[ch].setTreble (*trebleParam);
        tone[ch].processBlock (x, numSamples);

        FloatVectorOperations::multiply (x, -1.0f, numSamples); // inverting amplifier
        FloatVectorOperations::add (x, -2.25f, numSamples); // add bias offset
        FloatVectorOperations::clip (x, x, -8.6f, 16.2f, numSamples); // op-amp clipping

        outProc[ch].setLevel (*levelParam);
        outProc[ch].processBlock (x, numSamples);
    }

    // downsample
    os.processSamplesDown (block);

    scope->pushSamples (buffer);
}

AudioProcessorEditor* ChowCentaur::createEditor()
{
    return new foleys::MagicPluginEditor (magicState, BinaryData::gui_xml, BinaryData::gui_xmlSize);
}

// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ChowCentaur();
}
