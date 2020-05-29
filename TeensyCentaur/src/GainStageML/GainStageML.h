#ifndef GAINSTAGEML_H_INCLUDED
#define GAINSTAGEML_H_INCLUDED

#include "dense.h"
#include "gru.h"
#include "gru.cpp"

template<int size, typename T>
struct GainStageModel
{
    const T GRUKernelWeights[1][3*size];
    const T GRURecurrentWeights[size][3*size];
    const T GRUBias[2][3*size];
    const T DenseWeights[size];
    const T DenseBias;
};

template<int size=16>
class GainStageML
{
public:
    GainStageML (const GainStageModel<size,float>& gsm)
    {
        gruLayer.setWVals(gsm.GRUKernelWeights);
        gruLayer.setUVals(gsm.GRURecurrentWeights);
        gruLayer.setBVals(gsm.GRUBias);

        denseLayer.setWeights(gsm.DenseWeights);
        denseLayer.setBias(gsm.DenseBias);
    }

    void reset()
    {
        gruLayer.reset();
    }

    inline float processSample(float x)
    {
        float input[1] = { x };
        gruLayer.forward(input, hidden);
        return denseLayer.forward(hidden);
    }

    void processBlock(float* block, const int numSamples)
    {
        for(int n = 0; n < numSamples; ++n)
            block[n] = processSample(block[n]);
    }

private:
    float hidden[size] = {0};

    GRULayer<1, size, float> gruLayer;
    Dense1<size, float> denseLayer;
};

#endif // GAINSTAGEML_H_INCLUDED
