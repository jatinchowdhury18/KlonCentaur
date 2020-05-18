#ifndef OUTPOUTSTAGEPROCESSOR_H_INCLUDED
#define OUTPOUTSTAGEPROCESSOR_H_INCLUDED

#include "IIRFilter.h"

class OutputStageProc : public IIRFilterN<1>
{
public:
    OutputStageProc()
    {
        levelSmooth = 1.0f;
    }

    void reset (float sampleRate)
    {
        IIRFilterN::reset();
        fs = (float) sampleRate;

        levelSmooth.setCurrentAndTargetValue (levelSmooth.getTargetValue());
        levelSmooth.reset (sampleRate, 0.05);

        calcCoefs (levelSmooth.getTargetValue());
    }

    void setLevel (float level)
    {
        levelSmooth.setTargetValue (jlimit (0.00001f, 1.0f, level));
    }

    void calcCoefs (float curLevel)
    {
        const float R1 = 560.0f + (1.0f - curLevel) * 10000.0f;
        const float R2 = curLevel * 10000.0f + 1.0f;
        constexpr float C1 = (float) 4.7e-6;

        // analog coefficients
        const auto b0s = C1 * R2;
        const auto b1s = 0.0f;
        const auto a0s = C1 * (R1 + R2);
        const auto a1s = 1.0f;

        // bilinear transform
        const auto K = 2.0f * fs;
        const auto a0 = a0s * K + a1s;
        b[0] = ( b0s * K + b1s) / a0;
        b[1] = (-b0s * K + b1s) / a0;
        a[0] = 1.0f;
        a[1] = (-a0s * K + a1s) / a0;
    }

    void processBlock (float* block, const int numSamples) noexcept override
    {
        if (levelSmooth.isSmoothing())
        {
            for (int n = 0; n < numSamples; ++n)
            {
                calcCoefs (levelSmooth.getNextValue());
                block[n] = processSample (block[n]);
            }
        }
        else
        {
            IIRFilterN::processBlock (block, numSamples);
        }
    }

private:
    float fs = 44100.0f;

    SmoothedValue<float, ValueSmoothingTypes::Multiplicative> levelSmooth;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OutputStageProc)
};

#endif // OUTPOUTSTAGEPROCESSOR_H_INCLUDED
