#pragma once

#include "JuceHeader.h"
#include "PluginBase.h"
#include "OutputStageWDF.h"

class OutputStage : public PluginBase<OutputStage>
{
public:
    OutputStage();
    ~OutputStage();
    
    static void addParameters (Parameters& params);
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock (AudioBuffer<float>& buffer) override;

private:
    std::atomic<float>* levelParam;

    OutputStageWDF outWDF[2];

    dsp::Oversampling<float> os { 2, 1, dsp::Oversampling<float>::FilterType::filterHalfBandPolyphaseIIR };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OutputStage)
};
