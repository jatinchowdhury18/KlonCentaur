#ifndef SUMMINGAMP_H_INCLUDED
#define SUMMINGAMP_H_INCLUDED

#include "SharedJuceHeader.h"
#include "IIRFilter.h"

namespace GainStageSpace
{
class SummingAmp : public IIRFilterN<1>
{
public:
    SummingAmp() {}

    void reset (float sampleRate)
    {
        IIRFilterN::reset();
        fs = (float) sampleRate;

        calcCoefs ();
    }

    void calcCoefs()
    {
        constexpr float R20 = (float) 392e3;
        constexpr float C13 = (float) 820e-12;

        // analog coefficients
        const auto b0s = 0.0f;
        const auto b1s = R20;
        const auto a0s = C13 * R20;
        const auto a1s = 1.0f;

        const auto K = 2.0f * fs;
        const auto a0 = a0s * K + a1s;
        b[0] = ( b0s * K + b1s) / a0;
        b[1] = (-b0s * K + b1s) / a0;
        a[0] = 1.0f;
        a[1] = (-a0s * K + a1s) / a0;
    }

private:
    float fs = 44100.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SummingAmp)
};
}

#endif // SUMMINGAMP_H_INCLUDED
