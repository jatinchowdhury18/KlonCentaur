cmake_minimum_required(VERSION 3.4)

project("CentaurBench")

# FRUT stuff
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/../modules/FRUT/prefix/FRUT/cmake")
include(Reprojucer)

jucer_project_begin(
    JUCER_VERSION "5.4.7"
    PROJECT_FILE "${CentaurBench_jucer_FILE}"
    PROJECT_ID "gSweW"
)

jucer_project_settings(
    PROJECT_NAME "CentaurBench"
    PROJECT_VERSION "1.0.0"
    COMPANY_NAME "chowdsp"
    COMPANY_COPYRIGHT "Jatin Chowdhury"
    REPORT_JUCE_APP_USAGE OFF
    DISPLAY_THE_JUCE_SPLASH_SCREEN OFF
    PROJECT_TYPE "Console Application"
    BUNDLE_IDENTIFIER "com.chowdsp.CentaurBench"
    CXX_LANGUAGE_STANDARD "C++17"
)

jucer_project_files("shared"
# Compile   Xcode     Binary    File
#           Resource  Resource
  x         .         .         "${CMAKE_CURRENT_LIST_DIR}/Main.cpp"
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
    JUCE_PLUGINHOST_VST OFF
    JUCE_PLUGINHOST_VST3 ON
    JUCE_PLUGINHOST_AU ON
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

jucer_export_target(
    "Xcode (MacOSX)"
)

jucer_export_target_configuration(
    "Xcode (MacOSX)"
    NAME "Debug"
    DEBUG_MODE ON
    BINARY_NAME "CentaurBench"
)

jucer_export_target_configuration(
    "Xcode (MacOSX)"
    NAME "Release"
    DEBUG_MODE OFF
    BINARY_NAME "CentaurBench"
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
    BINARY_NAME "CentaurBench"
)

jucer_export_target_configuration(
    "Visual Studio 2017"
    NAME "Release"
    DEBUG_MODE OFF
    BINARY_NAME "CentaurBench"
)

jucer_export_target(
    "Linux Makefile"
)

jucer_export_target_configuration(
    "Linux Makefile"
    NAME "Debug"
    DEBUG_MODE ON
    BINARY_NAME "CentaurBench"
)

jucer_export_target_configuration(
    "Linux Makefile"
    NAME "Release"
    DEBUG_MODE OFF
    BINARY_NAME "CentaurBench"
)

jucer_project_end()
