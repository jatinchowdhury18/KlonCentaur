# On Linux, use JUCE with LV2 capability
if(UNIX AND NOT APPLE)
    add_subdirectory(distrho-juce)
else()
    add_subdirectory(JUCE)
endif()

juce_add_modules(foleys_gui_magic)
juce_add_modules(chowdsp_utils)
add_subdirectory(RTNeural)
