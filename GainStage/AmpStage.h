#ifndef AMPSTAGE_H_INCLUDED
#define AMPSTAGE_H_INCLUDED

#include "IIRFilter.h"

namespace GainStageSpace
{
class AmpStage : public IIRFilterN<2>
{
public:
    AmpStage()
    {
        r10bSmooth = 2000.0f;
    }

    void setGain (float gain)
    {
        float newR10b = (1.0f - gain) * 100000.0f + 2000.0f;
        r10bSmooth.setTargetValue (jlimit (2000.0f, 102000.0f, newR10b));
    }

    void reset (float sampleRate)
    {
        IIRFilterN::reset();
        fs = (float) sampleRate;

        r10bSmooth.setCurrentAndTargetValue (r10bSmooth.getTargetValue());
        r10bSmooth.reset (sampleRate, 0.05);

        calcCoefs (r10bSmooth.getTargetValue());
    }

    static inline float calcPoleFreq (float a, float b, float c)
    {
        auto radicand = b*b - 4.0f*a*c;
        if (radicand >= 0.0f)
            return 0.0f;

        return std::sqrt (-radicand) / (2.0f * a);
    }

    void calcCoefs (float curR10b)
    {
        // component values
        constexpr float C7 = (float) 82e-9;
        constexpr float C8 = (float) 390e-12;

        // analog coeffs
        const float a0s = C7 * C8 * curR10b * R11 * R12;
        const float a1s = C7 * curR10b * R11 + C8 * R12 * (curR10b + R11);
        const float a2s = curR10b + R11;
        const float b0s = a0s;
        const float b1s = C7 * R11 * R12 + a1s;
        const float b2s = R12 + a2s;

        // frequency warping
        const float wc = calcPoleFreq (a0s, a1s, a2s);
        const auto K = wc == 0.0f ? 2.0f * fs : wc / std::tan (wc / (2.0f * fs));
        const auto KSq = K * K;

        // bilinear transform
        const float a0 = a0s * KSq + a1s * K + a2s;
        a[0] = a0 / a0;
        a[1] = 2.0f * (a2s - a0s * KSq) / a0;
        a[2] = (a0s * KSq - a1s * K + a2s) / a0;
        b[0] = (b0s * KSq + b1s * K + b2s) / a0;
        b[1] = 2.0f * (b2s - b0s * KSq) / a0;
        b[2] = (b0s * KSq - b1s * K + b2s) / a0;
    }

    void processBlock (float* block, const int numSamples) noexcept override
    {
        if (r10bSmooth.isSmoothing())
        {
            for (int n = 0; n < numSamples; ++n)
            {
                auto curR10b = r10bSmooth.getNextValue();
                calcCoefs (curR10b);
                block[n] = processSample (block[n]);

#ifndef NO_BIAS
                block[n] += 4.5f * R12 / (R12 + R11 + curR10b); // bias
#endif
            }
        }
        else
        {
            IIRFilterN::processBlock (block, numSamples);
#ifndef NO_BIAS
            FloatVectorOperations::add (block,
                4.5f * R12 / (R12 + R11 + r10bSmooth.getCurrentValue()), numSamples); // bias
#endif
        }
    }

private:
    const float R11 = (float) 15e3;
    const float R12 = (float) 422e3;

    float fs = 44100.0f;
    SmoothedValue<float, ValueSmoothingTypes::Multiplicative> r10bSmooth;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AmpStage)
};
} // namespace

#endif // AMPSTAGE_H_INCLUDED
