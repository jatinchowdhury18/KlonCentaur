#include "ToneFilterProcessor.h"

void ToneFilterProcessor::prepare (float sampleRate)
{
    chowdsp::IIRFilter<1>::reset();
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
    const float G2 = 1.0f / ((float) 1.8e3 + (1.0f - curTreble) * Rpot);
    const float G3 = 1.0f / ((float) 4.7e3 + curTreble * Rpot);
    constexpr float G4 = 1.0f / (float) 100e3;

    constexpr float wc = G1 / C; // frequency to match
    const auto K = wc / std::tan (wc / (2.0f * fs)); // frequency warp to match transition freq

    // analog coefficients
    float as[2], bs[2];
    bs[0] = C * (G1 + G2);
    bs[1] = G1 * (G2 + G3);
    as[0] = C * (G3 - G4);
    as[1] = -G4 * (G2 + G3);

    // bilinear transform
    float aU[2], bU[2];
    chowdsp::Bilinear::BilinearTransform<float, 2>::call (bU, aU, bs, as, K);

    // flip pole inside unit circle to ensure stability
    a[0] = 1.0f;
    a[1] = 1.0f / aU[1];
    b[0] = bU[0] / aU[1];
    b[1] = bU[1] / aU[1];
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
        chowdsp::IIRFilter<1>::processBlock (block, numSamples);
    }
}
