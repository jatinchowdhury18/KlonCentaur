#ifndef GAINSTAGEMLPROC_H_INCLUDED
#define GAINSTAGEMLPROC_H_INCLUDED

#include "JuceHeader.h"
#include "Json2RnnParser.h"

class GainStageMLProc
{
public:
    GainStageMLProc (AudioProcessorValueTreeState& vts);

    void reset (double sampleRate, int samplesPerBlock);
    void processBlock (AudioBuffer<float>& buffer);

private:
    using ModelPtr = std::unique_ptr<Model<float>>;
    void loadModel (ModelPtr model[2], const char* data, int size);

    std::atomic<float>* gainParam = nullptr;
    std::atomic<float>* mlParam = nullptr;

    ModelPtr gainStageML[2];
    ModelPtr gainStageMLLarge[2];

    float T = 1.0f / 44100.0f;
};

#endif // GAINSTAGEMLPROC_H_INCLUDED
