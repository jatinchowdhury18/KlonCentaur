#ifndef IIRFILTER_H_INCLUDED
#define IIRFILTER_H_INCLUDED

#include "JuceHeader.h"

/** IIR filter of arbirtary order */
template<int order, typename FloatType=float>
class IIRFilterN
{
public:
    IIRFilterN() {}

    virtual void reset()
    {
        for(int i = 0; i <= order; ++i)
            z[i] = 0.0f;
    }

    /** Uses Transposed Direct Form II */
    inline virtual FloatType processSample (FloatType x) noexcept
    {
        FloatType y = z[1] + x * b[0];

        for (int i = 1; i < order; ++i)
            z[i] = z[i+1] + x * b[i] - y * a[i];

        z[order] = x * b[order] - y * a[order];

        return y;
    }

    virtual void processBlock (FloatType* block, const int numSamples) noexcept
    {
        for (int n = 0; n < numSamples; ++n)
            block[n] = processSample (block[n]);
    }

protected:
    FloatType a[order+1];
    FloatType b[order+1];

private:
    FloatType z[order+1];

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (IIRFilterN)
};

#endif // IIRFILTER_H_INCLUDED
