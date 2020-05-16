#ifndef TONEFILTERPROCESSOR_H_INCLUDED
#define TONEFILTERPROCESSOR_H_INCLUDED

#include "IIRFilter.h"

class ToneFilterProcessor  : public IIRFilterN<1>
{
public:
    ToneFilterProcessor()
    {
        trebleDBSmooth = 0.0f;
    }

    void setTreble (float trebleGainDB)
    {
        trebleDBSmooth.setTargetValue (trebleGainDB);
    }

    void reset (float sampleRate)
    {
        IIRFilterN::reset();
        fs = (float) sampleRate;

        trebleDBSmooth.setCurrentAndTargetValue (trebleDBSmooth.getTargetValue());
        trebleDBSmooth.reset (sampleRate, 0.05);

        calcCoefs (trebleDBSmooth.getTargetValue());
    }

    /** Calculate coefs for first order shelf with
     * Bass gain = 0dB and treble gain = curTrebleDB
     */
    void calcCoefs (float curTrebleDB)
    {
        const auto curTrebleGain = Decibels::decibelsToGain (curTrebleDB);
        if (curTrebleGain == 1.0f) // pass through
        {
            b[0] = 1.0f; b[1] = 0.0f;
            a[0] = 1.0f, a[1] = 0.0f;
            return;
        }

        constexpr float fc = 408.0f; // transition freq.
        const auto wc = MathConstants<float>::twoPi * fc;
        const auto p = std::sqrt (wc*wc * (curTrebleGain*curTrebleGain - curTrebleGain) / (curTrebleGain - 1.0f));
        const auto K = p / std::tan (p / (2.0f * fs)); // frequency warp to match transition freq

        // analog coefficients
        const auto b0s = curTrebleGain / p;
        const auto b1s = 1.0f;
        const auto a0s = 1.0f / p;
        const auto a1s = 1.0f;
        
        // bilinear transform
        const auto a0 = a0s * K + a1s;
        b[0] = ( b0s * K + b1s) / a0;
        b[1] = (-b0s * K + b1s) / a0;
        a[0] = 1.0f;
        a[1] = (-a0s * K + a1s) / a0;
    }

    void processBlock (float* block, const int numSamples) noexcept override
    {
        if (trebleDBSmooth.isSmoothing())
        {
            for (int n = 0; n < numSamples; ++n)
            {
                calcCoefs (trebleDBSmooth.getNextValue());
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

    SmoothedValue<float, ValueSmoothingTypes::Linear> trebleDBSmooth;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ToneFilterProcessor)
};

#endif // TONEFILTERPROCESSOR_H_INCLUDED
