#ifndef GAINSTAGEMLPROC_H_INCLUDED
#define GAINSTAGEMLPROC_H_INCLUDED

#include "JuceHeader.h"
#include <RTNeural/RTNeural.h>

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

    using ModelType = RTNeural::ModelT<float,
                                       RTNeural::GRULayer<float>,
                                       RTNeural::Dense<float>>;

    std::vector<std::vector<ModelType>> gainStageML;

    void loadModel (std::vector<ModelType>& model, const char* data, int size);
    void processModel (AudioBuffer<float>& buffer, std::vector<ModelType>& model);

    inline int getModelIdx() const noexcept
    {
        return jlimit (0, 4, int (numModels * *gainParam));
    }

    AudioBuffer<float> fadeBuffer;
    std::atomic<float>* gainParam = nullptr;
    int lastModelIdx = 0;
};

#endif // GAINSTAGEMLPROC_H_INCLUDED
