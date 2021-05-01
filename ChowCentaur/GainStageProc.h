#ifndef GAINSTAGEPROC_H_INCLUDED
#define GAINSTAGEPROC_H_INCLUDED

#include "SHaredCode/AmpStage.h"
#include "SHaredCode/ClippingStage.h"
#include "SHaredCode/FeedForward2.h"
#include "SHaredCode/PreAmpStage.h"
#include "SHaredCode/SummingAmp.h"

// #include "JuceHeader.h"

class GainStageProc
{
public:
    GainStageProc (AudioProcessorValueTreeState& vts);

    void reset (double sampleRate, int samplesPerBlock);
    void processBlock (AudioBuffer<float>& buffer);

private:
    std::atomic<float>* gainParam = nullptr;

    std::unique_ptr<GainStageSpace::PreAmpWDF> preAmp[2];
    GainStageSpace::AmpStage amp[2];
    std::unique_ptr<GainStageSpace::ClippingWDF> clip[2];
    std::unique_ptr<GainStageSpace::FeedForward2WDF> ff2[2];
    GainStageSpace::SummingAmp sumAmp[2];

    AudioBuffer<float> ff1Buff;
    AudioBuffer<float> ff2Buff;
    dsp::Oversampling<float> os { 2, 1, dsp::Oversampling<float>::FilterType::filterHalfBandPolyphaseIIR };
};

#endif // GAINSTAGEPROC_H_INCLUDED
