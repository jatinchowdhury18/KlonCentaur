#ifndef SUMMINGAMP_H_INCLUDED
#define SUMMINGAMP_H_INCLUDED

#include <Audio.h>

class SummingAmp : public AudioFilterBiquad
{
public:
    SummingAmp()
    {
        double coefficients[5];

        // component values
        constexpr float R20 = (float) 392e3;
        constexpr float C13 = (float) 820e-12;

        // analog coefficients
        const auto b0s = 0.0f;
        const auto b1s = R20;
        const auto a0s = C13 * R20;
        const auto a1s = 1.0f;

        const auto K = 2.0f * AUDIO_SAMPLE_RATE_EXACT;
        const auto a0 = a0s * K + a1s;
        
        /* b0 */ coefficients[0] = ( b0s * K + b1s) / a0;
		/* b1 */ coefficients[1] = (-b0s * K + b1s) / a0;
		/* b2 */ coefficients[2] = 0.0;
		/* a1 */ coefficients[3] = (-a0s * K + a1s) / a0;
		/* a2 */ coefficients[4] = 0.0;

        setCoefficients(0, coefficients);
    }
};

#endif // SUMMINGAMP_H_INCLUDED
