#ifndef GAINSTAGEPROC_H_INCLUDED
#define GAINSTAGEPROC_H_INCLUDED

#include "AmpStage.h"
#include "ClippingStage.h"
#include "FeedForward2.h"
#include "JuceHeader.h"
#include "PreAmpStage.h"
#include "SummingAmp.h"

class GainStageProc
{
public:
    GainStageProc (AudioProcessorValueTreeState& vts);

    void reset (double sampleRate, int samplesPerBlock);
    void processBlock (AudioBuffer<float>& buffer);

private:
    std::atomic<float>* gainParam = nullptr;

    GainStageSpace::PreAmpWDF preAmp[2];
    GainStageSpace::AmpStage amp[2];
    GainStageSpace::ClippingWDF clip[2];
    GainStageSpace::FeedForward2WDF ff2[2];
    GainStageSpace::SummingAmp sumAmp[2];

    AudioBuffer<float> ff1Buff;
    AudioBuffer<float> ff2Buff;
    dsp::Oversampling<float> os { 2, 1, dsp::Oversampling<float>::FilterType::filterHalfBandPolyphaseIIR };
};

#endif // GAINSTAGEPROC_H_INCLUDED
