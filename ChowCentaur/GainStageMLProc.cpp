#include "GainStageMLProc.h"

GainStageMLProc::GainStageMLProc (AudioProcessorValueTreeState& vts)
{
    MemoryInputStream jsonInput0 (BinaryData::centaur_json, BinaryData::centaur_jsonSize, false);
    MemoryInputStream jsonInput1 (BinaryData::centaur_json, BinaryData::centaur_jsonSize, false);

    Json2RnnParser parser;
    gainStageML[0] = parser.parseJson (jsonInput0);
    gainStageML[1] = parser.parseJson (jsonInput1);

    jassert (gainStageML[0].get() != nullptr);
    jassert (gainStageML[1].get() != nullptr);

    gainParam = vts.getRawParameterValue ("gain");
}

void GainStageMLProc::reset (double sampleRate, int samplesPerBlock)
{
    ignoreUnused (samplesPerBlock);

    for (int ch = 0; ch < 2; ++ch)
        gainStageML[ch]->reset();

    T = 1.0f / (float) sampleRate;
}

void GainStageMLProc::processBlock (AudioBuffer<float>& buffer)
{
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        auto* x = buffer.getWritePointer (ch);

        for (int n = 0; n < buffer.getNumSamples(); ++n)
        {
            float input[] = { x[n], *gainParam, T };
            x[n] = gainStageML[ch]->forward (input);
        }
    }
}
