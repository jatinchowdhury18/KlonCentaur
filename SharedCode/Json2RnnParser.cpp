#include "Json2RnnParser.h"

std::unique_ptr<Model<float>> Json2RnnParser::parseJson (InputStream& input)
{
    auto parent = JSON::parse (input);
    auto shape = parent["in_shape"];
    auto layers = parent["layers"];

    if (! shape.isArray() || ! layers.isArray())
        return nullptr;
    
    auto nDims = int (shape.getArray()->getUnchecked (2));
    // std::cout << "# dimensions: " << nDims <<std::endl;
    auto model = std::make_unique<Model<float>> (nDims);

    for (int i = 0; i < layers.getArray()->size(); ++i)
    {
        auto l = layers.getArray()->getUnchecked (i);
        const auto type = l["type"].toString();
        
        const auto layerShape = l["shape"];
        auto layerDims = int (layerShape.getArray()->getUnchecked (2));

        auto weights = l["weights"];
        // std::cout << type << " " << layerDims << std::endl;

        if (type == "time-distributed-dense" || type == "dense")
        {
            auto dense = createDense (model->getNextInSize(), layerDims, weights);
            model->addLayer (dense.release());

            if (type == "time-distributed-dense")
            {
                auto activation = std::make_unique<TanhActivation<float>> (layerDims);
                model->addLayer (activation.release());
            }
        }
        else if (type == "gru")
        {
            auto gru = createGRU (model->getNextInSize(), layerDims, weights);
            model->addLayer (gru.release());
        }
        else
        {
            jassertfalse;
        }
    }
    
    return std::move (model); //.release();
}

std::unique_ptr<Dense<float>> Json2RnnParser::createDense (size_t in_size, size_t out_size, var& weights)
{
    auto dense = std::make_unique<Dense<float>> (in_size, out_size);
    
    // load kernel weights
    float** denseWeights;
    denseWeights = new float* [out_size];
    for (size_t i = 0; i < out_size; ++i)
        denseWeights[i] = new float[in_size];
        
    auto layerWeights = weights.getArray()->getUnchecked (0);
    for (int i = 0; i < layerWeights.getArray()->size(); ++i)
    {
        auto lw = layerWeights.getArray()->getUnchecked (i);
        for (int j = 0; j < lw.getArray()->size(); ++j)
            denseWeights[j][i] = (float) (double) lw.getArray()->getUnchecked (j);
    }

    dense->setWeights (denseWeights);

    for (size_t i = 0; i < out_size; ++i)
        delete[] denseWeights[i];
    delete[] denseWeights;

    // load bias
    std::vector<float> denseBias (out_size, 0.0f);
    auto layerBiases = weights.getArray()->getUnchecked (1);
    for (int i = 0; i < layerBiases.getArray()->size(); ++i)
        denseBias[i] = (float) (double) layerBiases.getArray()->getUnchecked (i);

    dense->setBias (denseBias.data());

    return std::move (dense);
}

std::unique_ptr<GRULayer<float>> Json2RnnParser::createGRU (size_t in_size, size_t out_size, var& weights)
{
    auto gru = std::make_unique<GRULayer<float>> (in_size, out_size);

    // load kernel weights
    float** kernelWeights;
    kernelWeights = new float* [in_size];
    for (size_t i = 0; i < in_size; ++i)
        kernelWeights[i] = new float[3*out_size];
        
    auto layerWeights = weights.getArray()->getUnchecked (0);
    for (int i = 0; i < layerWeights.getArray()->size(); ++i)
    {
        auto lw = layerWeights.getArray()->getUnchecked (i);
        for (int j = 0; j < lw.getArray()->size(); ++j)
            kernelWeights[i][j] = (float) (double) lw.getArray()->getUnchecked (j);
    }

    gru->setWVals (kernelWeights);

    for (size_t i = 0; i < in_size; ++i)
        delete[] kernelWeights[i];
    delete[] kernelWeights;

    // load recurrent weights
    float** recurrentWeights;
    recurrentWeights = new float* [out_size];
    for (size_t i = 0; i < out_size; ++i)
        recurrentWeights[i] = new float[3*out_size];
        
    auto layerWeights2 = weights.getArray()->getUnchecked (1);
    for (int i = 0; i < layerWeights2.getArray()->size(); ++i)
    {
        auto lw = layerWeights2.getArray()->getUnchecked (i);
        for (int j = 0; j < lw.getArray()->size(); ++j)
            recurrentWeights[i][j] = (float) (double) lw.getArray()->getUnchecked (j);
    }

    gru->setUVals (recurrentWeights);

    for (size_t i = 0; i < out_size; ++i)
        delete[] recurrentWeights[i];
    delete[] recurrentWeights;

    // load biases
    float** gruBias;
    gruBias = new float* [2];
    for (size_t i = 0; i < 2; ++i)
        gruBias[i] = new float[3*out_size];
        
    auto layerBias = weights.getArray()->getUnchecked (2);
    for (int i = 0; i < layerBias.getArray()->size(); ++i)
    {
        auto lw = layerBias.getArray()->getUnchecked (i);
        for (int j = 0; j < lw.getArray()->size(); ++j)
            gruBias[i][j] = (float) (double) lw.getArray()->getUnchecked (j);
    }

    gru->setBVals (gruBias);

    for (size_t i = 0; i < 2; ++i)
        delete[] gruBias[i];
    delete[] gruBias;

    return std::move (gru);
}
