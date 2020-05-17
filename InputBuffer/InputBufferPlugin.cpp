#include "InputBufferPlugin.h"

InputBuffer::InputBuffer()
{
}

InputBuffer::~InputBuffer()
{
}

void InputBuffer::addParameters (Parameters& params)
{
}

void InputBuffer::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    PluginBase::prepareToPlay (sampleRate, samplesPerBlock);

    os.initProcessing (samplesPerBlock);

    for (int ch = 0; ch < 2; ++ch)
        ibWDF[ch].reset (sampleRate* os.getOversamplingFactor());
}

void InputBuffer::releaseResources()
{
    os.reset();
}

void InputBuffer::processBlock (AudioBuffer<float>& buffer)
{
    ScopedNoDenormals noDenormals;

    dsp::AudioBlock<float> block (buffer);
    dsp::AudioBlock<float> osBlock (buffer);

    // upsample
    osBlock = os.processSamplesUp (block);
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        auto* x = osBlock.getChannelPointer (ch);

        // process with WDF
        for (int n = 0; n < osBlock.getNumSamples(); ++n)
            x[n] = ibWDF[ch].processSample (x[n]);

        // op amp clip
        FloatVectorOperations::clip (x, x, 0.0f, 9.0f, (int) osBlock.getNumSamples());
    }

    // downsample
    os.processSamplesDown (block);
}

// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new InputBuffer();
}
