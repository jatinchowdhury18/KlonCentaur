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

    using ModelPtr = std::unique_ptr<RTNeural::Model<float>>;
    void loadModel (ModelPtr model[2], const char* data, int size);
    void processModel (AudioBuffer<float>& buffer, ModelPtr model[2]);

    inline int getModelIdx() const noexcept
    {
        return jlimit (0, 4, int (numModels * *gainParam));
    }

    ModelPtr gainStageML[numModels][2];

    AudioBuffer<float> fadeBuffer;
    std::atomic<float>* gainParam = nullptr;
    int lastModelIdx = 0;
};

#endif // GAINSTAGEMLPROC_H_INCLUDED
