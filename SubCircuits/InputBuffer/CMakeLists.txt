juce_add_plugin(InputBuffer
    COMPANY_NAME chowdsp
    PLUGIN_MANUFACTURER_CODE Chow
    PLUGIN_CODE Cib1
    FORMATS AU VST3 Standalone # VST
    ProductName "InputBuffer"
)

add_dependencies(SharedCode InputBuffer)

juce_generate_juce_header(InputBuffer)

target_sources(InputBuffer PRIVATE
    InputBufferPlugin.h
    InputBufferPlugin.cpp
)

target_compile_definitions(InputBuffer PUBLIC
    JUCE_DISPLAY_SPLASH_SCREEN=0
    JUCE_REPORT_APP_USAGE=0
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
    FOLEYS_SHOW_GUI_EDITOR_PALLETTE=0
)

target_link_libraries(InputBuffer PRIVATE
    juce::juce_audio_utils
    foleys_gui_magic
    SharedCode
)
