juce_add_console_app(CentaurBench
    PRODUCT_NAME "CentaurBench"
)

juce_generate_juce_header(CentaurBench)
add_custom_command(TARGET CentaurBench
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo "copying $<TARGET_FILE:CentaurBench> to ${PROJECT_BINARY_DIR}/CentaurBenchExe"
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:CentaurBench> ${PROJECT_BINARY_DIR}/CentaurBenchExe)


target_sources(CentaurBench PRIVATE
    Main.cpp
)

target_compile_definitions(CentaurBench PUBLIC
    JUCE_DISPLAY_SPLASH_SCREEN=0
    JUCE_REPORT_APP_USAGE=0
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
    JucePlugin_Manufacturer="chowdsp"
    JucePlugin_VersionString="${CMAKE_PROJECT_VERSION}"
    JucePlugin_Name="${CMAKE_PROJECT_NAME}"
)

target_include_directories(CentaurBench PUBLIC modules)
target_include_directories(CentaurBench PUBLIC ../ChowCentaur)

target_link_libraries(CentaurBench PRIVATE
    BinaryData
    juce::juce_audio_utils
    foleys_gui_magic
    chowdsp_utils
    ChowCentaur
)
