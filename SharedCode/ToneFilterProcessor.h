#ifndef TONEFILTERPROCESSOR_H_INCLUDED
#define TONEFILTERPROCESSOR_H_INCLUDED

#include "IIRFilter.h"
#include "SharedJuceHeader.h"

class ToneFilterProcessor : public IIRFilterN<1>
{
public:
    ToneFilterProcessor()
    {
        trebleSmooth = 0.287394f;
    }

    void setTreble (float treble)
    {
        trebleSmooth.setTargetValue (jlimit (0.0f, 1.0f, treble));
    }

    void reset (float sampleRate);
    void calcCoefs (float curTreble);
    void processBlock (float* block, const int numSamples) noexcept override;

private:
    float fs = 44100.0f;

    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> trebleSmooth;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ToneFilterProcessor)
};

#endif // TONEFILTERPROCESSOR_H_INCLUDED
