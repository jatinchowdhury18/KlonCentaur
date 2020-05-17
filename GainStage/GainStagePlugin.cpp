#include "GainStagePlugin.h"

GainStage::GainStage()
{
    gainParam = vts.getRawParameterValue ("gain");
}

GainStage::~GainStage()
{
}

void GainStage::addParameters (Parameters& params)
{
    params.push_back (std::make_unique<AudioParameterFloat> ("gain", "Gain", 0.0f, 1.0f, 0.5f));
}

void GainStage::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    os.initProcessing (samplesPerBlock);
    const auto osFactor = os.getOversamplingFactor();

    for (int ch = 0; ch < 2; ++ch)
    {
        preAmpStage[ch].reset (sampleRate * osFactor);
        ampStage[ch].reset ((float) sampleRate * osFactor);
        clippingStage[ch].reset (sampleRate * osFactor);
        ff2[ch].reset (sampleRate * osFactor);
        sumAmp[ch].reset ((float) sampleRate * osFactor);
    }

    ff1Buff.setSize (2, samplesPerBlock * (int) osFactor);
    ff2Buff.setSize (2, samplesPerBlock * (int) osFactor);
}

void GainStage::releaseResources()
{
    os.reset();
}

void GainStage::processBlock (AudioBuffer<float>& buffer)
{
    ScopedNoDenormals noDenormals;

    // process main signal path
    dsp::AudioBlock<float> block (buffer);
    dsp::AudioBlock<float> osBlock (buffer);

    // upsample
    osBlock = os.processSamplesUp (block);
    const auto numSamples = osBlock.getNumSamples();
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        auto* x = osBlock.getChannelPointer (ch);
        auto* x1 = ff1Buff.getWritePointer (ch);

        // copy buffers for side chain 2
        FloatVectorOperations::copy (ff2Buff.getWritePointer (ch), x, (int) numSamples);

        preAmpStage[ch].setGain (*gainParam);
        for (int n = 0; n < numSamples; ++n)
        {
            x[n] = preAmpStage[ch].processSample (x[n]);
            x1[n] = preAmpStage[ch].getFF1();
        }

        ampStage[ch].setGain (*gainParam);
        ampStage[ch].processBlock (x, (int) numSamples);
        // FloatVectorOperations::clip (x, x, 0.0f, 9.0f, numSamples);

        for (int n = 0; n < numSamples; ++n)
            x[n] = clippingStage[ch].processSample (x[n]);
    }

    // process FF 2
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        auto* x = ff2Buff.getWritePointer (ch);

        ff2[ch].setGain (*gainParam);
        for (int n = 0; n < numSamples; ++n)
            x[n] = ff2[ch].processSample (x[n]);
    }

    // summing amp
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        auto* x  = osBlock.getChannelPointer (ch);
        auto* x1 = ff1Buff.getWritePointer   (ch);
        auto* x2 = ff2Buff.getWritePointer   (ch);

        FloatVectorOperations::add (x, x1, (int) numSamples);
        FloatVectorOperations::add (x, x2, (int) numSamples);
        sumAmp[ch].processBlock (x, (int) numSamples);
        // FloatVectorOperations::clip (x, x, -8.6f, 16.2f, numSamples);
    }

    // downsample
    os.processSamplesDown (block);
}

// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new GainStage();
}
