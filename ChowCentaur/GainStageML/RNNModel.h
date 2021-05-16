#pragma once

#include <pch.h>

// useful for debugging and performance comparisons
#define USE_NEW_MODEL 1

namespace RNNSpace
{
using v_type = dsp::SIMDRegister<float>;
using v_simd_type = v_type::vSIMDType;
constexpr auto v_size = v_type::SIMDNumElements;
static_assert (v_size == 4, "SIMD float size is required to be 4.");
using x_type = xsimd::batch<float, v_size>;

class Gru18
{
public:
    static constexpr size_t in_size = 1;
    static constexpr size_t out_size = 8;
    static constexpr size_t v_out_size = out_size / 4;

    Gru18() = default;

    void setKernelWeights (const std::vector<std::vector<float>>& w);
    void setRecurrentWeights (const std::vector<std::vector<float>>& w);
    void setBias (const std::vector<std::vector<float>>& w);

    void reset();

    inline void mul8 (const v_type (&vec)[v_out_size], const v_type (&mat)[out_size][v_out_size], v_type (&out)[v_out_size])
    {
        for (size_t i = 0; i < v_size; ++i)
        {
            out[0].set (i, (mat[i][0] * vec[0]).sum() + (mat[i][1] * vec[1]).sum());
            out[1].set (i, (mat[i + v_size][0] * vec[0]).sum() + (mat[i + v_size][1] * vec[1]).sum());
        }
    }

    static inline void sigmoid (v_type& x) noexcept
    {
        x.value = (v_simd_type) (1.0f / (1.0f + xsimd::exp (-(x_type (x.value)))));
    }

    inline void forward (float input)
    {
        x = v_type (input);

        // compute zt
        mul8 (outs, Uz, zt);
        zt[0] = zt[0] + bz[0] + (Wz[0] * x);
        zt[1] = zt[1] + bz[1] + (Wz[1] * x);
        sigmoid (zt[0]);
        sigmoid (zt[1]);

        // compute rt
        mul8 (outs, Ur, rt);
        rt[0] = rt[0] + br[0] + (Wr[0] * x);
        rt[1] = rt[1] + br[1] + (Wr[1] * x);
        sigmoid (rt[0]);
        sigmoid (rt[1]);

        // compute h_hat
        mul8 (outs, Uh, ct);
        ht[0] = rt[0] * (ct[0] + bh1[0]) + bh0[0] + (Wh[0] * x);
        ht[1] = rt[1] * (ct[1] + bh1[1]) + bh0[1] + (Wh[1] * x);
        ht[0].value = (v_simd_type) xsimd::tanh ((x_type) ht[0].value);
        ht[1].value = (v_simd_type) xsimd::tanh ((x_type) ht[1].value);

        // compute output
        outs[0] = (v_type (1.0f) - zt[0]) * ht[0] + zt[0] * outs[0];
        outs[1] = (v_type (1.0f) - zt[1]) * ht[1] + zt[1] * outs[1];
    }

    // output state
    v_type outs[v_out_size];

private:
    // input
    v_type x;

    // kernel weights
    v_type Wz[v_out_size];
    v_type Wr[v_out_size];
    v_type Wh[v_out_size];

    // recurrent weights
    v_type Uz[out_size][v_out_size];
    v_type Ur[out_size][v_out_size];
    v_type Uh[out_size][v_out_size];

    // biases
    v_type bz[v_out_size];
    v_type br[v_out_size];
    v_type bh0[v_out_size];
    v_type bh1[v_out_size];

    // intermediate vars
    v_type zt[v_out_size];
    v_type rt[v_out_size];
    v_type ct[v_out_size];
    v_type ht[v_out_size];
};

class Dense81
{
public:
    static constexpr size_t in_size = 8;
    static constexpr size_t v_in_size = in_size / v_size;
    static constexpr size_t out_size = 1;

    Dense81() = default;

    void setWeights (std::vector<std::vector<float>>& w);
    void setBias (std::vector<float>& b);

    inline float forward (const v_type (&ins)[v_in_size]) const noexcept
    {
        float out = 0.0f;
        for (size_t k = 0; k < v_in_size; ++k)
            out += (ins[k] * weights[k]).sum();

        return out + bias;
    }

private:
    float bias;
    v_type weights[2];
};

} // namespace RNNSpace

class RNNModel
{
public:
    RNNModel() = default;

    inline float forward (float x) noexcept
    {
#if USE_NEW_MODEL
        float input alignas (16)[] { x };
        return model.forward (input);
#else
        gru.forward (x);
        return dense.forward (gru.outs);
#endif
    }

    void reset();
    void loadModel (const nlohmann::json& modelJ);

private:
#if USE_NEW_MODEL
    using ModelType = RTNeural::ModelT<float, 1, 1,
                                       RTNeural::GRULayerT<float, 1, 8>,
                                       RTNeural::DenseT<float, 8, 1>>;
    ModelType model;
#else
    RNNSpace::Gru18 gru;
    RNNSpace::Dense81 dense;
#endif

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RNNModel)
};
