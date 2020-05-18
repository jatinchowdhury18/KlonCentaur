#pragma once

#include "JuceHeader.h"
#include "PluginBase.h"
#include "ToneFilterProcessor.h"

class ToneFilter : public PluginBase<ToneFilter>
{
public:
    ToneFilter();
    ~ToneFilter();
    
    static void addParameters (Parameters& params);
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock (AudioBuffer<float>& buffer) override;

private:
    std::atomic<float>* trebleParam = nullptr;

    ToneFilterProcessor filter[2];

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ToneFilter)
};
