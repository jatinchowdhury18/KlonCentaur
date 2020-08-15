#include "GainStageMLProc.h"

GainStageMLProc::GainStageMLProc (AudioProcessorValueTreeState& vts)
{
    loadModel (gainStageML, BinaryData::centaur_json, BinaryData::centaur_jsonSize);
    loadModel (gainStageMLLarge, BinaryData::centaur_large_json, BinaryData::centaur_large_jsonSize);

    gainParam = vts.getRawParameterValue ("gain");
    mlParam     = vts.getRawParameterValue ("neural");
}

void GainStageMLProc::loadModel (ModelPtr model[2], const char* data, int size)
{
    MemoryInputStream jsonInput0 (data, size, false);
    MemoryInputStream jsonInput1 (data, size, false);

    Json2RnnParser parser;
    model[0] = parser.parseJson (jsonInput0);
    model[1] = parser.parseJson (jsonInput1);

    jassert (model[0].get() != nullptr);
    jassert (model[1].get() != nullptr);
}

void GainStageMLProc::reset (double sampleRate, int samplesPerBlock)
{
    ignoreUnused (samplesPerBlock);

    for (int ch = 0; ch < 2; ++ch)
    {
        gainStageML[ch]->reset();
        gainStageMLLarge[ch]->reset();
    }

    T = 1.0f / (float) sampleRate;
}

void GainStageMLProc::processBlock (AudioBuffer<float>& buffer)
{
    auto model = gainStageML;
    if (*mlParam == 2)
        model = gainStageMLLarge;

    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        auto* x = buffer.getWritePointer (ch);

        for (int n = 0; n < buffer.getNumSamples(); ++n)
        {
            float input[] = { x[n], *gainParam, T };
            x[n] = model[ch]->forward (input);
        }
    }
}
