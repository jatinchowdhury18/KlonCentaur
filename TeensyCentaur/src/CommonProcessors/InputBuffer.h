#ifndef INPUT_BUFFER_H_INCLUDED
#define INPUT_BUFFER_H_INCLUDED

#include <Audio.h>

class InputBuffer : public AudioFilterBiquad
{
public:
    InputBuffer()
    {
        double coefficients[5];

        // component values
        constexpr float R1 = 10000.0f;
        constexpr float R2 = 1000000.0f;
        constexpr float C1 = (float) 0.1e-6;

        // analog coefficients
        const auto b0s = C1 * R2;
        const auto b1s = 0.0f;
        const auto a0s = C1 * (R1 + R2);
        const auto a1s = 1.0f;

        // bilinear transform
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

#endif // INPUT_BUFFER_H_INCLUDED
