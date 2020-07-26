#include "ToneFilterProcessor.h"

void ToneFilterProcessor::reset (float sampleRate)
{
    IIRFilterN::reset();
    fs = (float) sampleRate;

    trebleSmooth.setCurrentAndTargetValue (trebleSmooth.getTargetValue());
    trebleSmooth.reset (sampleRate, 0.05);

    calcCoefs (trebleSmooth.getTargetValue());
}

void ToneFilterProcessor::calcCoefs (float curTreble)
{
    constexpr float Rpot = (float) 10e3;
    constexpr float C = (float) 3.9e-9;
    constexpr float G1 = 1.0f / (float) 100e3;
    const float G2 = 1.0f / ((float) 1.8e3 + (1.0f-curTreble)*Rpot);
    const float G3 = 1.0f / ((float) 4.7e3 + curTreble*Rpot);
    constexpr float G4 = 1.0f / (float) 100e3;

    constexpr float wc = G1 / C; // frequency to match
    const auto K = wc / std::tan (wc / (2.0f * fs)); // frequency warp to match transition freq

    // analog coefficients
    const auto b0s = C * (G1 + G2);
    const auto b1s = G1 * (G2 + G3);
    const auto a0s = C * (G3 - G4);
    const auto a1s = -G4 * (G2 + G3);
        
    // bilinear transform
    const auto a0 = a0s * K + a1s;
    const float bU0 = ( b0s * K + b1s) / a0;
    const float bU1 = (-b0s * K + b1s) / a0;
    const float aU0 = 1.0f;
    const float aU1 = (-a0s * K + a1s) / a0;

    // flip pole inside unit circle to ensure stability
    a[0] = 1.0f;
    a[1] = 1.0f / aU1;
    b[0] = bU0 / aU1;
    b[1] = bU1 / aU1;
}

void ToneFilterProcessor::processBlock (float* block, const int numSamples) noexcept
{
    if (trebleSmooth.isSmoothing())
    {
        for (int n = 0; n < numSamples; ++n)
        {
            calcCoefs (trebleSmooth.getNextValue());
            block[n] = processSample (block[n]);
        }
    }
    else
    {
        IIRFilterN::processBlock (block, numSamples);
    }
}
