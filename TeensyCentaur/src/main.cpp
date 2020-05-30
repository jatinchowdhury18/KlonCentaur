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
#else
#include "GainStageML/GainStageMLProc.h"
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
#else
GainStageMLProc gainStageRNN;
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
AudioConnection patchRNN (inputBuffer, 0, gainStageRNN, 0);
AudioConnection patchTone (gainStageRNN, 0, toneControl, 0);
#endif

AudioConnection patchToneOut (toneControl, 0, outputBuffer, 0);
AudioConnection patchOut (outputBuffer, 0, amp, 0);
AudioConnection patchCord1 (amp, 0, out, 0);
AudioConnection patchCord2 (amp, 0, out, 1);

// button stuff
#define GAIN_PIN   PIN_A2
#define TREBLE_PIN PIN_A3
#define TREBLE_LIGHT 0

double gainVal = 1.0;

int trebleOn = 0;
int gainOn = 0;

int gainButtonVal = 0;
int trebleButtonVal = 0;

// ARDUINO functions
void setup()
{
    AudioMemory(6);
    audioShield.enable();
    audioShield.volume(0.9);

#ifndef USE_ML
    amp.gain(4.0);
#else
    amp.gain(0.5);
#endif

    toneControl.setTreble (0.4);
    outputBuffer.setLevel (0.5);

#ifndef USE_ML
    preAmpStage.setGain (gainVal);
    ampStage.setGain (gainVal);
    ff2.setGain (gainVal);
#else
    gainStageRNN.setGain (gainVal);
#endif

    pinMode (LED_BUILTIN, OUTPUT);
    pinMode (TREBLE_LIGHT, OUTPUT);
    pinMode (GAIN_PIN,   INPUT_PULLUP);
    pinMode (TREBLE_PIN, INPUT_PULLUP);
}

void loop()
{
    // update gain state
    int curGainButtonVal = analogRead (GAIN_PIN);
    if (gainButtonVal - curGainButtonVal > 300)
        gainOn = 1 - gainOn;

    gainButtonVal = curGainButtonVal;

    // update treble state
    int curTrebleButtonVal = analogRead (TREBLE_PIN);
    if (trebleButtonVal - curTrebleButtonVal > 300)
        trebleOn = 1 - trebleOn;

    trebleButtonVal = curTrebleButtonVal;

    // apply treble param
    if (trebleOn)
    {
        digitalWrite (TREBLE_LIGHT, HIGH);
        toneControl.setTreble (0.7);
    }
    else
    {
        digitalWrite (TREBLE_LIGHT, LOW);
        toneControl.setTreble (0.2);
    }

    // apply gain param
    if (gainOn)
    {
        digitalWrite (LED_BUILTIN, HIGH);
        gainVal = 1.0;
    }
    else
    {
        digitalWrite (LED_BUILTIN, LOW);
        gainVal = 0.0;
    }

#ifndef USE_ML
    preAmpStage.setGain (gainVal);
    ampStage.setGain (gainVal);
    ff2.setGain (gainVal);
#else
    gainStageRNN.setGain (gainVal);
#endif
}
