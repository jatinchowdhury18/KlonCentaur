cmake_minimum_required(VERSION 3.4)

project("SharedCode")

# FRUT stuff
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/../modules/FRUT/prefix/FRUT/cmake")
include(Reprojucer)

jucer_project_begin(
    JUCER_VERSION "5.4.7"
    PROJECT_FILE "${SharedCode_jucer_FILE}"
    PROJECT_ID "aSweT"
)

jucer_project_settings(
    PROJECT_NAME "SharedCode"
    PROJECT_VERSION "1.0.0"
    COMPANY_NAME "chowdsp"
    COMPANY_COPYRIGHT "Jatin Chowdhury"
    REPORT_JUCE_APP_USAGE OFF
    DISPLAY_THE_JUCE_SPLASH_SCREEN OFF
    PROJECT_TYPE "Static Library"
    BUNDLE_IDENTIFIER "com.chowdsp.SharedCode"
    CXX_LANGUAGE_STANDARD "C++17"
)

jucer_project_files("shared"
# Compile   Xcode     Binary    File
#           Resource  Resource
  .         .         .         "${CMAKE_CURRENT_LIST_DIR}/PluginBase.h"
  .         .         .         "${CMAKE_CURRENT_LIST_DIR}/IIRFilter.h"
  .         .         .         "${CMAKE_CURRENT_LIST_DIR}/InputBufferProcessor.h"
  .         .         .         "${CMAKE_CURRENT_LIST_DIR}/PreAmpStage.h"
  .         .         .         "${CMAKE_CURRENT_LIST_DIR}/AmpStage.h"
  .         .         .         "${CMAKE_CURRENT_LIST_DIR}/ClippingStage.h"
  .         .         .         "${CMAKE_CURRENT_LIST_DIR}/FeedForward2.h"
  .         .         .         "${CMAKE_CURRENT_LIST_DIR}/SummingAmp.h"
  .         .         .         "${CMAKE_CURRENT_LIST_DIR}/ToneFilterProcessor.h"
  .         .         .         "${CMAKE_CURRENT_LIST_DIR}/OutputStageProcessor.h"
)

# JUCE modules
jucer_project_module(
    juce_audio_basics
    PATH "${CMAKE_CURRENT_LIST_DIR}/../modules/JUCE/modules"
)

jucer_project_module(
    juce_audio_devices
    PATH "${CMAKE_CURRENT_LIST_DIR}/../modules/JUCE/modules"
)

jucer_project_module(
    juce_audio_formats
    PATH "${CMAKE_CURRENT_LIST_DIR}/../modules/JUCE/modules"
)

jucer_project_module(
    juce_audio_processors
    PATH "${CMAKE_CURRENT_LIST_DIR}/../modules/JUCE/modules"
)

jucer_project_module(
    juce_audio_utils
    PATH "${CMAKE_CURRENT_LIST_DIR}/../modules/JUCE/modules"
)

jucer_project_module(
    juce_core
    PATH "${CMAKE_CURRENT_LIST_DIR}/../modules/JUCE/modules"
)

jucer_project_module(
    juce_data_structures
    PATH "${CMAKE_CURRENT_LIST_DIR}/../modules/JUCE/modules"
)

jucer_project_module(
    juce_dsp
    PATH "${CMAKE_CURRENT_LIST_DIR}/../modules/JUCE/modules"
)

jucer_project_module(
    juce_events
    PATH "${CMAKE_CURRENT_LIST_DIR}/../modules/JUCE/modules"
)

jucer_project_module(
    juce_graphics
    PATH "${CMAKE_CURRENT_LIST_DIR}/../modules/JUCE/modules"
)

jucer_project_module(
    juce_gui_basics
    PATH "${CMAKE_CURRENT_LIST_DIR}/../modules/JUCE/modules"
)

jucer_project_module(
    juce_gui_extra
    PATH "${CMAKE_CURRENT_LIST_DIR}/../modules/JUCE/modules"
)

jucer_project_module(
    foleys_gui_magic
    PATH "$\{CMAKE_CURRENT_LIST_DIR\}/../modules"
    FOLEYS_SHOW_GUI_EDITOR_PALLETTE OFF # Turn on to edit your GUI!
)

jucer_export_target(
    "Xcode (MacOSX)"
)

jucer_export_target_configuration(
    "Xcode (MacOSX)"
    NAME "Debug"
    DEBUG_MODE ON
    BINARY_NAME "SharedCode"
)

jucer_export_target_configuration(
    "Xcode (MacOSX)"
    NAME "Release"
    DEBUG_MODE OFF
    BINARY_NAME "SharedCode"
)

jucer_export_target(
    "Visual Studio 2017"
    EXTRA_PREPROCESSOR_DEFINITIONS
        _CRT_SECURE_NO_WARNINGS
)

jucer_export_target_configuration(
    "Visual Studio 2017"
    NAME "Debug"
    DEBUG_MODE ON
    BINARY_NAME "SharedCode"
)

jucer_export_target_configuration(
    "Visual Studio 2017"
    NAME "Release"
    DEBUG_MODE OFF
    BINARY_NAME "SharedCode"
)

jucer_export_target(
    "Linux Makefile"
)

jucer_export_target_configuration(
    "Linux Makefile"
    NAME "Debug"
    DEBUG_MODE ON
    BINARY_NAME "SharedCode"
)

jucer_export_target_configuration(
    "Linux Makefile"
    NAME "Release"
    DEBUG_MODE OFF
    BINARY_NAME "SharedCode"
)

jucer_project_end()
