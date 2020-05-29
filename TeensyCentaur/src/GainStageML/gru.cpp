#include "gru.h"

template <int in_size, int out_size, typename T>
void GRULayer<in_size, out_size, T>::reset()
{
    std::fill(ht1, ht1 + out_size, (T) 0);
}

template <int in_size, int out_size, typename T>
void GRULayer<in_size, out_size, T>::setWVals(const T wVals [in_size][3*out_size])
{
    for(int i = 0; i < in_size; ++i)
    {
        for(int k = 0; k < out_size; ++k)
        {
            zWeights.W[k][i] = wVals[i][k];
            rWeights.W[k][i] = wVals[i][k+out_size];
            cWeights.W[k][i] = wVals[i][k+out_size*2];
        }
    }
}

template <int in_size, int out_size, typename T>
void GRULayer<in_size, out_size, T>::setUVals(const T uVals [out_size][3*out_size])
{
    for(int i = 0; i < out_size; ++i)
    {
        for(int k = 0; k < out_size; ++k)
        {
            zWeights.U[k][i] = uVals[i][k];
            rWeights.U[k][i] = uVals[i][k+out_size];
            cWeights.U[k][i] = uVals[i][k+out_size*2];
        }
    }
}

template <int in_size, int out_size, typename T>
void GRULayer<in_size, out_size, T>::setBVals(const T bVals [2][3*out_size])
{
    for(int i = 0; i < 2; ++i)
    {
        for(int k = 0; k < out_size; ++k)
        {
            zWeights.b[i][k] = bVals[i][k];
            rWeights.b[i][k] = bVals[i][k+out_size];
            cWeights.b[i][k] = bVals[i][k+out_size*2];
        }
    }
}

