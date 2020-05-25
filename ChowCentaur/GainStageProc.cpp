#include "GainStageProc.h"

GainStageProc::GainStageProc (AudioProcessorValueTreeState& vts)
{
    gainParam = vts.getRawParameterValue ("gain");
}

void GainStageProc::reset (double sampleRate, int samplesPerBlock)
{
    os.initProcessing (samplesPerBlock);

    const auto osFactor = (int) os.getOversamplingFactor();
    for (int ch = 0; ch < 2; ++ch)
    {
        preAmp[ch].reset (sampleRate * osFactor);
        amp[ch].reset ((float) sampleRate * osFactor);
        clip[ch].reset (sampleRate * osFactor);
        ff2[ch].reset (sampleRate * osFactor);
        sumAmp[ch].reset ((float) sampleRate * osFactor);
    }

    ff1Buff.setSize (2, samplesPerBlock * osFactor);
    ff2Buff.setSize (2, samplesPerBlock * osFactor);
}

void GainStageProc::processBlock (AudioBuffer<float>& buffer)
{
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
        FloatVectorOperations::clip (x, x, -4.5f, 4.5f, numSamples);

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
        FloatVectorOperations::clip (x, x, -13.1f, 11.7f, numSamples);
    }

    // downsample
    os.processSamplesDown (block);
}
