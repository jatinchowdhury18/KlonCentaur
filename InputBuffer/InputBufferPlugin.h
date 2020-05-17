#pragma once

#include "JuceHeader.h"
#include "PluginBase.h"
#include "InputBufferWDF.h"
#include "InputBufferProcessor.h"

class InputBuffer : public PluginBase<InputBuffer>
{
public:
    InputBuffer();
    ~InputBuffer();
    
    static void addParameters (Parameters& params);
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock (AudioBuffer<float>& buffer) override;

private:
    InputBufferWDF ibWDF[2];
    InputBufferProcessor inProc[2];

    dsp::Oversampling<float> os { 2, 1, dsp::Oversampling<float>::FilterType::filterHalfBandPolyphaseIIR };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (InputBuffer)
};
