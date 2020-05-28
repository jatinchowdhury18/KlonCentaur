#ifndef AMPSTAGE_H_INCLUDED
#define AMPSTAGE_H_INCLUDED

#include <Audio.h>

class AmpStage : public AudioFilterBiquad
{
public:
    AmpStage()
    {
        setGain (0.5);
    }

    static inline float calcPoleFreq (float a, float b, float c)
    {
        auto radicand = b*b - 4.0f*a*c;
        if (radicand >= 0.0f)
            return 0.0f;

        return sqrt (-radicand) / (2.0f * a);
    }

    void setGain(double gain)
    {
        double coefficients[5];

        // component values
        const float curR10b = (1.0f - gain) * 100000.0f + 2000.0f;
        constexpr float C7 = (float) 82e-9;
        constexpr float C8 = (float) 390e-12;
        constexpr float R11 = (float) 15e3;
        constexpr float R12 = (float) 422e3;

        // analog coeffs
        const float a0s = C7 * C8 * curR10b * R11 * R12;
        const float a1s = C7 * curR10b * R11 + C8 * R12 * (curR10b + R11);
        const float a2s = curR10b + R11;
        const float b0s = a0s;
        const float b1s = C7 * R11 * R12 + a1s;
        const float b2s = R12 + a2s;

        // frequency warping
        const float wc = calcPoleFreq (a0s, a1s, a2s);
        const auto K = wc == 0.0f ? 2.0f * AUDIO_SAMPLE_RATE_EXACT
            : wc / tan (wc / (2.0f * AUDIO_SAMPLE_RATE_EXACT));
        const auto KSq = K * K;

        // bilinear transform
        const float a0 = a0s * KSq + a1s * K + a2s;
        /* b0 */ coefficients[0] = (b0s * KSq + b1s * K + b2s) / a0;
		/* b1 */ coefficients[1] = 2.0f * (b2s - b0s * KSq) / a0;
		/* b2 */ coefficients[2] = (b0s * KSq - b1s * K + b2s) / a0;
		/* a1 */ coefficients[3] = 2.0f * (a2s - a0s * KSq) / a0;
		/* a2 */ coefficients[4] = (a0s * KSq - a1s * K + a2s) / a0;

        setCoefficients(0, coefficients);
    }
};

#endif // AMPSTAGE_H_INCLUDED
