#include "GainStageMLProc.h"

GainStageMLProc::GainStageMLProc (AudioProcessorValueTreeState& vts)
{


    loadModel (gainStageML[0], BinaryData::centaur_0_json, BinaryData::centaur_0_jsonSize);
    loadModel (gainStageML[1], BinaryData::centaur_25_json, BinaryData::centaur_25_jsonSize);
    loadModel (gainStageML[2], BinaryData::centaur_50_json, BinaryData::centaur_50_jsonSize);
    loadModel (gainStageML[3], BinaryData::centaur_75_json, BinaryData::centaur_75_jsonSize);
    loadModel (gainStageML[4], BinaryData::centaur_100_json, BinaryData::centaur_100_jsonSize);

    gainParam = vts.getRawParameterValue ("gain");
}

void GainStageMLProc::loadModel (ModelPair& model, const char* data, int size)
{
    MemoryInputStream jsonInputStream (data, size, false);
    auto jsonInput = nlohmann::json::parse (jsonInputStream.readEntireStreamAsString().toStdString());

    model[0].loadModel (jsonInput);
    model[1].loadModel (jsonInput);
}

void GainStageMLProc::reset (double sampleRate, int samplesPerBlock)
{
    ignoreUnused (sampleRate);

    for (int i = 0; i < numModels; ++i)
    {
        for (int ch = 0; ch < 2; ++ch)
        {
            gainStageML[i][ch].reset();

            // pre-buffer to avoid "click" on initialisation
            for (int k = 0; k < 2048; ++k)
                gainStageML[i][ch].forward (0.0f);
        }
    }

    fadeBuffer.setSize (2, samplesPerBlock);

    lastModelIdx = getModelIdx();
}

void GainStageMLProc::processModel (AudioBuffer<float>& buffer, ModelPair& model)
{
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        auto* x = buffer.getWritePointer (ch);

        for (int n = 0; n < buffer.getNumSamples(); ++n)
            x[n] = model[ch].forward (x[n]);
    }
}

void GainStageMLProc::processBlock (AudioBuffer<float>& buffer)
{
    const auto modelIdx = getModelIdx();

    if (modelIdx == lastModelIdx)
    {
        processModel (buffer, gainStageML[modelIdx]);
    }
    else // need to fade between models
    {
        fadeBuffer.makeCopyOf (buffer, true);
        processModel (buffer, gainStageML[lastModelIdx]); // previous model
        processModel (fadeBuffer, gainStageML[modelIdx]); // next model

        buffer.applyGainRamp (0, buffer.getNumSamples(), 1.0f, 0.0f);
        for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
            buffer.addFromWithRamp (ch, 0, fadeBuffer.getReadPointer (ch), buffer.getNumSamples(), 0.0f, 1.0f);
    }

    lastModelIdx = modelIdx;
}
