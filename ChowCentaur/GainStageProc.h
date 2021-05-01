#ifndef GAINSTAGEPROC_H_INCLUDED
#define GAINSTAGEPROC_H_INCLUDED

#include "GainStageProcessors/AmpStage.h"
#include "GainStageProcessors/ClippingStage.h"
#include "GainStageProcessors/FeedForward2.h"
#include "GainStageProcessors/PreAmpStage.h"
#include "GainStageProcessors/SummingAmp.h"

class GainStageProc
{
public:
    GainStageProc (AudioProcessorValueTreeState& vts, double sampleRate);

    void reset (double sampleRate, int samplesPerBlock);
    void processBlock (AudioBuffer<float>& buffer);

private:
    std::atomic<float>* gainParam = nullptr;

    AudioBuffer<float> ff1Buff;
    AudioBuffer<float> ff2Buff;
    dsp::Oversampling<float> os { 2, 1, dsp::Oversampling<float>::FilterType::filterHalfBandPolyphaseIIR };

    GainStageSpace::PreAmpWDF preAmpL, preAmpR;
    GainStageSpace::PreAmpWDF* preAmp[2] { &preAmpL, &preAmpR };

    GainStageSpace::ClippingWDF clipL, clipR;
    GainStageSpace::ClippingWDF* clip[2] { &clipL, &clipR };

    GainStageSpace::FeedForward2WDF ff2L, ff2R;
    GainStageSpace::FeedForward2WDF* ff2[2] { &ff2L, &ff2R };

    GainStageSpace::AmpStage amp[2];
    GainStageSpace::SummingAmp sumAmp[2];
};

#endif // GAINSTAGEPROC_H_INCLUDED
