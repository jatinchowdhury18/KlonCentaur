#pragma once

#include "JuceHeader.h"
#include "PluginBase.h"
#include "AmpStage.h"
#include "PreAmpStage.h"
#include "ClippingStage.h"
#include "FeedForward2.h"
#include "SummingAmp.h"

#define NO_BIAS

using namespace GainStageSpace;

class GainStage : public PluginBase<GainStage>
{
public:
    GainStage();
    ~GainStage();

    static void addParameters (Parameters& params);
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock (AudioBuffer<float>& buffer) override;

private:
    std::atomic<float>* gainParam;

    PreAmpWDF preAmpStage[2];
    AmpStage ampStage[2];
    ClippingWDF clippingStage[2];
    FeedForward2WDF ff2[2];
    SummingAmp sumAmp[2];

    AudioBuffer<float> ff1Buff;
    AudioBuffer<float> ff2Buff;

    dsp::Oversampling<float> os { 2, 1, dsp::Oversampling<float>::filterHalfBandPolyphaseIIR };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GainStage)
};
