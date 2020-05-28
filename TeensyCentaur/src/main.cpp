#include "Arduino.h"
#include <Audio.h>
#include "CommonProcessors/InputBuffer.h"
#include "CommonProcessors/ToneControl.h"
#include "CommonProcessors/OutputBuffer.h"

// #define USE_ML

#ifndef USE_ML
#include "GainStageProcessors/PreAmpStage.h"
#include "GainStageProcessors/AmpStage.h"
#include "GainStageProcessors/SummingAmp.h"
#include "GainStageProcessors/ClippingStage.h"
#include "GainStageProcessors/FF1Current.h"
#include "GainStageProcessors/FeedForward2.h"
#endif

// Basic Audio objects
AudioInputI2S in;
AudioOutputI2S out;
AudioControlSGTL5000 audioShield;

// common processors
InputBuffer inputBuffer;
ToneControl toneControl;
OutputBuffer outputBuffer;

// Gain stage processors
#ifndef USE_ML
PreAmpStage preAmpStage;
AmpStage ampStage;
ClippingStage clippingStage;
FF1Current ff1 (preAmpStage);
FeedForward2 ff2;
AudioMixer4 mixer;
SummingAmp summingAmp;
#endif

// output amp?
AudioAmplifier amp;

// set up audio graph
AudioConnection patchCordIn (in, 0, inputBuffer, 0);

#ifndef USE_ML
AudioConnection patchpreAmp (inputBuffer, 0, preAmpStage, 0);
AudioConnection patchAmp (preAmpStage, 0, ampStage, 0);
AudioConnection patchClipping (ampStage, 0, clippingStage, 0);
AudioConnection patchFF2 (inputBuffer, 0, ff2, 0);

AudioConnection sum0 (ff1, 0, mixer, 0);
AudioConnection sum1 (clippingStage, 0, mixer, 1);
AudioConnection sum2 (ff2, 0, mixer, 2);
AudioConnection patchSummingAmp (mixer, 0, summingAmp, 0);
AudioConnection patchTone (summingAmp, 0, toneControl, 0);
#else
AudioConnection patchTone (inputBuffer, 0, toneControl, 0);
#endif

AudioConnection patchToneOut (toneControl, 0, outputBuffer, 0);
AudioConnection patchOut (outputBuffer, 0, amp, 0);
AudioConnection patchCord1 (amp, 0, out, 0);
AudioConnection patchCord2 (amp, 0, out, 1);

void setup()
{
    AudioMemory(6);
    audioShield.enable();
    audioShield.volume(0.9);

#ifndef USE_ML
    amp.gain(4.0);
#else
    amp.gain(1.0);
#endif

    toneControl.setTreble (0.4);
    outputBuffer.setLevel (0.5);

    double gainVal = 1.0;
#ifndef USE_ML
    preAmpStage.setGain (gainVal);
    ampStage.setGain (gainVal);
    ff2.setGain (gainVal);
#endif
}

void loop()
{
}
