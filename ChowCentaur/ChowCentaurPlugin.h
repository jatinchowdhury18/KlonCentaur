#pragma once

#include "JuceHeader.h"
#include "PluginBase.h"

#include "InputBufferProcessor.h"
#include "PreAmpStage.h"
#include "AmpStage.h"
#include "ClippingStage.h"
#include "FeedForward2.h"
#include "SummingAmp.h"
#include "ToneFilterProcessor.h"
#include "OutputStageProcessor.h"

#include "GainStageML.h"

class ChowCentaur : public PluginBase<ChowCentaur>
{
public:
    ChowCentaur();
    ~ChowCentaur();
    
    static void addParameters (Parameters& params);
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock (AudioBuffer<float>& buffer) override;

    AudioProcessorEditor* createEditor();

private:
    std::atomic<float>* gainParam   = nullptr;
    std::atomic<float>* trebleParam = nullptr;
    std::atomic<float>* levelParam  = nullptr;
    std::atomic<float>* mlParam     = nullptr;

    InputBufferProcessor inProc[2];
    GainStageSpace::PreAmpWDF preAmp[2];
    GainStageSpace::AmpStage amp[2];
    GainStageSpace::ClippingWDF clip[2];
    GainStageSpace::FeedForward2WDF ff2[2];
    GainStageSpace::SummingAmp sumAmp[2];
    ToneFilterProcessor tone[2];
    OutputStageProc outProc[2];

    AudioBuffer<float> ff1Buff;
    AudioBuffer<float> ff2Buff;
    dsp::Oversampling<float> os { 2, 1, dsp::Oversampling<float>::FilterType::filterHalfBandPolyphaseIIR };

    GainStageML gainStageML[2];

    foleys::MagicPlotSource* scope = nullptr;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ChowCentaur)
};
