#ifndef OUTPOUTSTAGEPROCESSOR_H_INCLUDED
#define OUTPOUTSTAGEPROCESSOR_H_INCLUDED

#include "SharedJuceHeader.h"
#include "IIRFilter.h"

class OutputStageProc : public IIRFilterN<1>
{
public:
    OutputStageProc()
    {
        levelSmooth = 1.0f;
    }

    void setLevel (float level)
    {
        levelSmooth.setTargetValue (jlimit (0.00001f, 1.0f, level));
    }

    void reset (float sampleRate);
    void calcCoefs (float curLevel);
    void processBlock (float* block, const int numSamples) noexcept override;

private:
    float fs = 44100.0f;

    SmoothedValue<float, ValueSmoothingTypes::Multiplicative> levelSmooth;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OutputStageProc)
};

#endif // OUTPOUTSTAGEPROCESSOR_H_INCLUDED
