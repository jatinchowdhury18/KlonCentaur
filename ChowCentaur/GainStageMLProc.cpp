#include "GainStageMLProc.h"
// #include "Models/Model_gain0.h"
// #include "Models/Model_gain25.h"
#include "Models/Model_gain50.h"
#include "Models/Model_gain75.h"
#include "Models/Model_gain100.h"

GainStageMLProc::GainStageMLProc (AudioProcessorValueTreeState& vts)
{
    for (int ch = 0; ch < 2; ++ch)
    {
        gainStageML[0][ch] = std::make_unique<GainStageML<8>> (ModelGain100);
        gainStageML[1][ch] = std::make_unique<GainStageML<8>> (ModelGain100);
        gainStageML[2][ch] = std::make_unique<GainStageML<8>> (ModelGain50);
        gainStageML[3][ch] = std::make_unique<GainStageML<8>> (ModelGain75);
        gainStageML[4][ch] = std::make_unique<GainStageML<8>> (ModelGain100);
    }

    gainParam = vts.getRawParameterValue ("gain");
}

void GainStageMLProc::reset (double sampleRate, int samplesPerBlock)
{
    ignoreUnused (sampleRate);

    lastModelIdx = jlimit (0, 4, int (5 * *gainParam));
    fadeBuffer.setSize (2, samplesPerBlock);

    for (int ch = 0; ch < 2; ++ch)
        for (int i = 0; i < 5; ++i)
            gainStageML[i][ch]->reset();
}

void GainStageMLProc::processModel (AudioBuffer<float>& buffer, int modelIdx)
{
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        auto* x = buffer.getWritePointer (ch);
        FloatVectorOperations::multiply (x, 2.0f, buffer.getNumSamples());
        gainStageML[modelIdx][ch]->processBlock (x, buffer.getNumSamples());
    }
}

void GainStageMLProc::processBlock (AudioBuffer<float>& buffer)
{
    const auto modelIdx = jlimit (0, 4, int (5 * *gainParam));

    if (modelIdx == lastModelIdx)
    {
        processModel (buffer, modelIdx);
    }
    else // need to fade between models
    {
        fadeBuffer.makeCopyOf (buffer, true);
        processModel (buffer, lastModelIdx); // previous model
        processModel (fadeBuffer, modelIdx); // next model

        buffer.applyGainRamp (0, buffer.getNumSamples(), 1.0f, 0.0f);
        for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
            buffer.addFromWithRamp (ch, 0, fadeBuffer.getReadPointer (ch), buffer.getNumSamples(), 0.0f, 1.0f);
    }

    lastModelIdx = modelIdx;
}
