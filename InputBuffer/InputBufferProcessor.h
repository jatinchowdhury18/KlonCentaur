#ifndef INPUTBUFFERPROCESSOR_H_INCLUDED
#define INPUTBUFFERPROCESSOR_H_INCLUDED

#include "IIRFilter.h"

class InputBufferProcessor : IIRFilterN<1>
{
public:
    InputBufferProcessor() {}

    void reset (float sampleRate)
    {
        IIRFilterN::reset();
        fs = sampleRate;

        calcCoefs();
    }

    void calcCoefs()
    {
        constexpr float R1 = 10000.0f;
        constexpr float R2 = 1000000.0f;
        constexpr float C1 = (float) 0.1e-6;

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

    void processBlock (float* buffer, const int numSamples) noexcept override
    {
        IIRFilterN::processBlock (buffer, numSamples);
        // FloatVectorOperations::add (buffer, 4.5, numSamples); // bias
    }

private:
    float fs = 44100.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (InputBufferProcessor)
};

#endif // INPUTBUFFERPROCESSOR_H_INCLUDED
