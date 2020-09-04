juce_add_console_app(CentaurBench
    PRODUCT_NAME "CentaurBench"
)

juce_generate_juce_header(CentaurBench)

target_sources(CentaurBench PRIVATE
    Main.cpp
)

target_compile_definitions(CentaurBench PUBLIC
    JUCE_DISPLAY_SPLASH_SCREEN=0
    JUCE_REPORT_APP_USAGE=0
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_PLUGINHOST_VST=0
    JUCE_PLUGINHOST_VST3=1
    JUCE_PLUGINHOST_AU=1
)

target_link_libraries(CentaurBench PRIVATE
    juce::juce_audio_utils
)
