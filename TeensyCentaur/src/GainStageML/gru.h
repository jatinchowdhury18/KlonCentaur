#ifndef GRU_H_INCLUDED
#define GRU_H_INCLUDED

#include <algorithm>
#include <numeric>
#include <cmath>
#include <cstring>

template <int in_size, int out_size, typename T = double>
class GRULayer
{
public:
    GRULayer() {}

    void reset();
    inline void forward(const T input[in_size],  T h[out_size])
    {
        for(int i = 0; i < out_size; ++i)
        {
            zVec[i] = sigmoid(vMult(zWeights.W[i], input, in_size) + vMult(zWeights.U[i], ht1, out_size) + zWeights.b[0][i] + zWeights.b[1][i]);
            rVec[i] = sigmoid(vMult(rWeights.W[i], input, in_size) + vMult(rWeights.U[i], ht1, out_size) + rWeights.b[0][i] + rWeights.b[1][i]);
            cVec[i] = std::tanh(vMult(cWeights.W[i], input, in_size) + rVec[i] * (vMult(cWeights.U[i], ht1, out_size) + cWeights.b[1][i]) + cWeights.b[0][i]);
            h[i] = ((T) 1 - zVec[i]) * cVec[i] + zVec[i] * ht1[i];
        }
    
        std::copy(h, h + out_size, ht1);
    }

    void setWVals(const T wVals [in_size][3*out_size]);
    void setUVals(const T uVals [out_size][3*out_size]);
    void setBVals(const T bVals [2][3*out_size]);

    inline T vMult(const T* arg1, const T* arg2, int dim)
    {
        return std::inner_product(arg1, arg1 + dim, arg2, (T) 0);
    }

    inline T sigmoid(T value)
    {
        return (T) 1 / ((T) 1 + std::exp(-value));
    }

private:
    T ht1[out_size];

    struct WeightSet
    {
        T W[out_size][in_size] = {{0}};
        T U[out_size][out_size] = {{0}};
        T b[2][out_size] = {{0}};
    };

    WeightSet zWeights;
    WeightSet rWeights;
    WeightSet cWeights;

    T zVec[out_size];
    T rVec[out_size];
    T cVec[out_size];
};

#endif // GRU_H_INCLUDED
