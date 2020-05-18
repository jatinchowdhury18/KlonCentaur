#include "OutputStagePlugin.h"

OutputStage::OutputStage()
{
    levelParam = vts.getRawParameterValue ("level");
}

OutputStage::~OutputStage()
{
}

void OutputStage::addParameters (Parameters& params)
{
    params.push_back (std::make_unique<AudioParameterFloat> ("level", "Level", 0.0f, 1.0f, 0.5f));
}

void OutputStage::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    PluginBase::prepareToPlay (sampleRate, samplesPerBlock);

    os.initProcessing (samplesPerBlock);

    for (int ch = 0; ch < 2; ++ch)
        outWDF[ch].reset (sampleRate* os.getOversamplingFactor());
}

void OutputStage::releaseResources()
{
    os.reset();
}

void OutputStage::processBlock (AudioBuffer<float>& buffer)
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
        outWDF[ch].setLevel (*levelParam);
        for (int n = 0; n < osBlock.getNumSamples(); ++n)
            x[n] = outWDF[ch].processSample (x[n]);
    }

    // downsample
    os.processSamplesDown (block);
}

// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new OutputStage();
}
