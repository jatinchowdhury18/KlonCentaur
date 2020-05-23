#ifndef DENSE_H_INCLUDED
#define DENSE_H_INCLUDED

#include <algorithm>

template<int in_size, typename T = double>
class Dense1
{
public:
    Dense1() {}

    inline T forward(const T input[in_size])
    {
        return std::inner_product(weights, weights + in_size, input, (T) 0) + bias;
    }

    void setWeights(const T newWeights[in_size])
    {
        for(int i = 0; i < in_size; ++i)
            weights[i] = newWeights[i];
    }

    void setBias(T b) { bias = b; } 

private:
    T weights[in_size];
    T bias;

};

#endif // DENSE_H_INCLUDED
