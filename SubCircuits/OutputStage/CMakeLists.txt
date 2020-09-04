juce_add_plugin(OutputStage
    COMPANY_NAME chowdsp
    PLUGIN_MANUFACTURER_CODE Chow
    PLUGIN_CODE Cus1
    FORMATS AU VST3 Standalone # VST
    ProductName "OutputStage"
)

add_dependencies(SharedCode OutputStage)

juce_generate_juce_header(OutputStage)

target_sources(OutputStage PRIVATE
    OutputStagePlugin.h
    OutputStagePlugin.cpp
)

target_compile_definitions(OutputStage PUBLIC
    JUCE_DISPLAY_SPLASH_SCREEN=0
    JUCE_REPORT_APP_USAGE=0
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
    FOLEYS_SHOW_GUI_EDITOR_PALLETTE=0
)

target_link_libraries(OutputStage PRIVATE
    juce::juce_audio_utils
    foleys_gui_magic
    SharedCode
)
