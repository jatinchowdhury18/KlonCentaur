#ifndef GAINSTAGEMLPROC_H_INCLUDED
#define GAINSTAGEMLPROC_H_INCLUDED

#include "JuceHeader.h"
#include "GainStageML.h"

class GainStageMLProc
{
public:
    GainStageMLProc (AudioProcessorValueTreeState& vts);

    void reset (double sampleRate, int samplesPerBlock);
    void processBlock (AudioBuffer<float>& buffer);
    void processModel (AudioBuffer<float>& buffer, int modelIdx);

private:
    std::atomic<float>* gainParam = nullptr;
    std::unique_ptr<GainStageML> gainStageML[5][2];

    AudioBuffer<float> fadeBuffer;
    int lastModelIdx = 0;
};

#endif // GAINSTAGEMLPROC_H_INCLUDED
