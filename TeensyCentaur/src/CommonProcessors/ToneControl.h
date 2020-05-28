#ifndef TONECONTROL_H_INCLUDED
#define TONECONTROL_H_INCLUDED

#include <Audio.h>

class ToneControl : public AudioFilterBiquad
{
public:
    ToneControl()
    {
        setTreble (0.5);
    }

    void setTreble (double treble)
    {
        double coefficients[5];

        constexpr float Rpot = (float) 10e3;
        constexpr float C = (float) 3.9e-9;
        constexpr float G1 = 1.0f / (float) 100e3;
        const float G2 = 1.0f / ((float) 1.8e3 + (1.0f-treble)*Rpot);
        const float G3 = 1.0f / ((float) 4.7e3 + treble*Rpot);
        constexpr float G4 = 1.0f / (float) 100e3;

        constexpr float wc = G1 / C; // frequency to match
        const auto K = wc / tan (wc / (2.0f * AUDIO_SAMPLE_RATE_EXACT)); // frequency warp to match transition freq

        // analog coefficients
        const auto b0s = C * (G1 + G2);
        const auto b1s = G1 * (G2 + G3);
        const auto a0s = C * (G3 - G4);
        const auto a1s = -G4 * (G2 + G3);
        
        // bilinear transform
        const auto a0 = a0s * K + a1s;
        const float bU0 = ( b0s * K + b1s) / a0;
        const float bU1 = (-b0s * K + b1s) / a0;
        const float aU1 = (-a0s * K + a1s) / a0;

        // flip pole inside unit circle to ensure stability
        /* b0 */ coefficients[0] = bU0 / aU1;
		/* b1 */ coefficients[1] = bU1 / aU1;
		/* b2 */ coefficients[2] = 0.0;
		/* a1 */ coefficients[3] = 1.0 / aU1;
		/* a2 */ coefficients[4] = 0.0;

        setCoefficients(0, coefficients);
    }
};

#endif // TONECONTROL_H_INCLUDED