#include "ChowCentaurPlugin.h"

namespace
{
    const String gainTag   = "gain";
    const String trebleTag = "treble";
    const String levelTag  = "level";
    const String neuralTag = "neural";
}

ChowCentaur::ChowCentaur()
{
    gainParam   = vts.getRawParameterValue (gainTag);
    trebleParam = vts.getRawParameterValue (trebleTag);
    levelParam  = vts.getRawParameterValue (levelTag);
    mlParam     = vts.getRawParameterValue (neuralTag);

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
    params.push_back (std::make_unique<AudioParameterBool>  (neuralTag, "Neural", false));
}

void ChowCentaur::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    os.initProcessing (samplesPerBlock);

    const auto osFactor = (int) os.getOversamplingFactor();
    for (int ch = 0; ch < 2; ++ch)
    {
        inProc[ch].reset ((float) sampleRate);
        preAmp[ch].reset (sampleRate * osFactor);
        amp[ch].reset ((float) sampleRate * osFactor);
        clip[ch].reset (sampleRate * osFactor);
        ff2[ch].reset (sampleRate * osFactor);
        sumAmp[ch].reset ((float) sampleRate * osFactor);
        tone[ch].reset ((float) sampleRate);
        outProc[ch].reset ((float) sampleRate);

        gainStageML[ch].reset();
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

    auto numSamples = buffer.getNumSamples();
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        auto* x = buffer.getWritePointer (ch);

        // Input buffer
        FloatVectorOperations::multiply (x, 0.5f, numSamples);
        inProc[ch].processBlock (x, numSamples);
        FloatVectorOperations::clip (x, x, -4.5f, 4.5f, numSamples); // op amp clipping
    }

    if (! *mlParam)
    {
        dsp::AudioBlock<float> block (buffer);
        dsp::AudioBlock<float> osBlock (buffer);

        // upsample
        osBlock = os.processSamplesUp (block);
        numSamples = (int) osBlock.getNumSamples();
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
    else
    {
        for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
        {
            auto* x = buffer.getWritePointer (ch);
            gainStageML[ch].processBlock (x, buffer.getNumSamples());
        }
    }
    

    numSamples = buffer.getNumSamples();
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
