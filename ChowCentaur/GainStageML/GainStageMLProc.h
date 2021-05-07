#ifndef GAINSTAGEMLPROC_H_INCLUDED
#define GAINSTAGEMLPROC_H_INCLUDED

#include "RNNModel.h"

class GainStageMLProc
{
public:
    GainStageMLProc (AudioProcessorValueTreeState& vts);

    void reset (double sampleRate, int samplesPerBlock);
    void processBlock (AudioBuffer<float>& buffer);

private:
    enum
    {
        numModels = 5,
    };

    using ModelPair = std::array<RNNModel, 2>;
    std::array<ModelPair, numModels> gainStageML;

    static void loadModel (ModelPair& model, const char* data, int size);
    void processModel (AudioBuffer<float>& buffer, ModelPair& model);

    inline int getModelIdx() const noexcept
    {
        return jlimit (0, numModels - 1, int ((float) numModels * *gainParam));
    }

    AudioBuffer<float> fadeBuffer;
    std::atomic<float>* gainParam = nullptr;
    int lastModelIdx = 0;
};

#endif // GAINSTAGEMLPROC_H_INCLUDED
