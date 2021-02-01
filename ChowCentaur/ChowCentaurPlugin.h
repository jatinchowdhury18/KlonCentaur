#pragma once

#include "JuceHeader.h"
#include "PluginBase.h"
#include "gui/MyLNF.h"

#include "InputBufferProcessor.h"
#include "ToneFilterProcessor.h"
#include "OutputStageProcessor.h"

#include "GainStageProc.h"
#include "GainStageMLProc.h"

class ChowCentaur : public PluginBase<ChowCentaur>
{
public:
    ChowCentaur();
    ~ChowCentaur();
    
    static void addParameters (Parameters& params);
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock (AudioBuffer<float>& buffer) override;

    AudioProcessorEditor* createEditor() override;
    void setStateInformation (const void* data, int sizeInBytes) override;

private:
    std::atomic<float>* trebleParam = nullptr;
    std::atomic<float>* levelParam  = nullptr;
    std::atomic<float>* mlParam     = nullptr;

    InputBufferProcessor inProc[2];
    ToneFilterProcessor tone[2];
    OutputStageProc outProc[2];

    GainStageProc gainStageProc;
    GainStageMLProc gainStageMLProc;

    MyLNF myLNF;
    foleys::MagicPlotSource* scope = nullptr;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ChowCentaur)
};
