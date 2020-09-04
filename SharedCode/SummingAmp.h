#ifndef SUMMINGAMP_H_INCLUDED
#define SUMMINGAMP_H_INCLUDED

#include "SharedJuceHeader.h"
#include "IIRFilter.h"
#include "BilinearTools.h"

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
        float as[2], bs[2];
        bs[0] = 0.0f;
        bs[1] = R20;
        as[0] = C13 * R20;
        as[1] = 1.0f;

        const auto K = 2.0f * fs;
        Bilinear::BilinearTransform<float, 2>::call (b, a, bs, as, K);
    }

private:
    float fs = 44100.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SummingAmp)
};
}

#endif // SUMMINGAMP_H_INCLUDED
