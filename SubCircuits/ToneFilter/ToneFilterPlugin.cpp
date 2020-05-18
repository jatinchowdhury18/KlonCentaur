#include "ToneFilterPlugin.h"

ToneFilter::ToneFilter()
{
    trebleParam = vts.getRawParameterValue ("treble");
}

ToneFilter::~ToneFilter()
{
}
    
void ToneFilter::addParameters (Parameters& params)
{
    params.push_back (std::make_unique<AudioParameterFloat> ("treble", "Treble", 0.0f, 1.0f, 0.287394f));
}

void ToneFilter::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    for (int ch = 0; ch < 2; ++ch)
        filter[ch].reset ((float) sampleRate);
}

void ToneFilter::releaseResources()
{
}

void ToneFilter::processBlock (AudioBuffer<float>& buffer)
{
    ScopedNoDenormals noDenormals;
    const auto numSamples = buffer.getNumSamples();

    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        auto* x = buffer.getWritePointer (ch);

        filter[ch].setTreble (*trebleParam);
        filter[ch].processBlock (x, numSamples);

        FloatVectorOperations::multiply (x, -1.0f, numSamples); // inverting amplifier
        FloatVectorOperations::add (x, -2.25f, numSamples); // add bias offset
        FloatVectorOperations::clip (x, x, -8.6f, 16.2f, numSamples); // op-amp clipping
    }
}

// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ToneFilter();
}
