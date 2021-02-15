#ifndef SHAREDCODE_JUCEHEADER_H_INCLUDED
#define SHAREDCODE_JUCEHEADER_H_INCLUDED

#if KLON_JUCER
#include <JuceHeader.h>
#else
#include "chowdsp_utils.h"
#include "foleys_gui_magic.h"
#include "juce_audio_utils/juce_audio_utils.h"

using namespace juce;
#endif // KLON_JUCER

#endif // SHAREDCODE_JUCEHEADER_H_INCLUDED
