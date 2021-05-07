#include "RNNModel.h"

namespace RNNSpace
{
void Gru18::reset()
{
    outs[0] = v_type::expand (0.0f);
    outs[1] = v_type::expand (0.0f);
}

void Gru18::setKernelWeights (const std::vector<std::vector<float>>& w)
{
    for (size_t i = 0; i < in_size; ++i)
    {
        for (size_t j = 0; j < out_size; ++j)
        {
            Wz[j / v_size].set (j % v_size, w[i][j]);
            Wr[j / v_size].set (j % v_size, w[i][j + out_size]);
            Wh[j / v_size].set (j % v_size, w[i][j + 2 * out_size]);
        }
    }
}

void Gru18::setRecurrentWeights (const std::vector<std::vector<float>>& w)
{
    for (size_t i = 0; i < out_size; ++i)
    {
        for (size_t j = 0; j < out_size; ++j)
        {
            Uz[i][j / v_size].set (j % v_size, w[i][j]);
            Ur[i][j / v_size].set (j % v_size, w[i][j + out_size]);
            Uh[i][j / v_size].set (j % v_size, w[i][j + 2 * out_size]);
        }
    }
}

void Gru18::setBias (const std::vector<std::vector<float>>& w)
{
    for(size_t k = 0; k < out_size; ++k)
    {
        bz[k / v_size].set (k % v_size, w[0][k] + w[1][k]);
        br[k / v_size].set (k % v_size, w[0][k + out_size] + w[1][k + out_size]);
        bh0[k / v_size].set (k % v_size, w[0][k + 2 * out_size]);
        bh1[k / v_size].set (k % v_size, w[1][k + 2 * out_size]);
    }
}

//========================================================================
void Dense81::setWeights (std::vector<std::vector<float>>& w)
{
    for (size_t i = 0; i < out_size; ++i)
        for (size_t j = 0; j < in_size; ++j)
            weights[j / v_size].set (j % v_size, w[i][j]);
}

void Dense81::setBias (std::vector<float>& b)
{
    bias = b[0];
}

} // namespace RNNSpace

void RNNModel::loadModel (const nlohmann::json& modelJ)
{
#if USE_RTNEURAL
    model.parseJson (modelJ);
#else
    using namespace RNNSpace;
    auto layers = modelJ["layers"];

    const auto weights_l0 = layers.at (0)["weights"];
    {
        // load kernel weights
        std::vector<std::vector<float>> kernelWeights(Gru18::in_size);
        for(auto& w : kernelWeights)
            w.resize(3 * Gru18::out_size, 0.0f);

        auto layerWeights = weights_l0[0];
        for(size_t i = 0; i < layerWeights.size(); ++i)
        {
            auto lw = layerWeights[i];
            for(size_t j = 0; j < lw.size(); ++j)
                kernelWeights[i][j] = lw[j].get<float>();
        }

        gru.setKernelWeights(kernelWeights);

        // load recurrent weights
        std::vector<std::vector<float>> recurrentWeights(Gru18::out_size);
        for(auto& w : recurrentWeights)
            w.resize(3 * Gru18::out_size, 0.0f);

        auto layerWeights2 = weights_l0[1];
        for(size_t i = 0; i < layerWeights2.size(); ++i)
        {
            auto lw = layerWeights2[i];
            for(size_t j = 0; j < lw.size(); ++j)
                recurrentWeights[i][j] = lw[j].get<float>();
        }

        gru.setRecurrentWeights(recurrentWeights);

        // load biases
        std::vector<std::vector<float>> gruBias(2);
        for(auto& b : gruBias)
            b.resize(3 * Gru18::out_size, 0.0f);

        auto layerBias = weights_l0[2];
        for(size_t i = 0; i < layerBias.size(); ++i)
        {
            auto lw = layerBias[i];
            for(size_t j = 0; j < lw.size(); ++j)
                gruBias[i][j] = lw[j].get<float>();
        }

        gru.setBias(gruBias);
    }

    const auto weights_l1 = layers.at (1)["weights"];
    {
        // load weights
        std::vector<std::vector<float>> denseWeights (Dense81::out_size);
        for (auto& w : denseWeights)
            w.resize (Dense81::in_size, 0.0);

        auto layerWeights = weights_l1[0];
        for (size_t i = 0; i < layerWeights.size(); ++i)
        {
            auto lw = layerWeights[i];
            for (size_t j = 0; j < lw.size(); ++j)
                denseWeights[j][i] = lw[j].get<float>();
        }
        dense.setWeights (denseWeights);

        // load biases
        std::vector<float> denseBias = weights_l1[1].get<std::vector<float>>();
        dense.setBias (denseBias);
    }
#endif
}

void RNNModel::reset()
{
#if USE_RTNEURAL
    model.reset();
#else
    gru.reset();
#endif
}
