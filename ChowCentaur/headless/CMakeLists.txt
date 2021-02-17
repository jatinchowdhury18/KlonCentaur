juce_add_console_app(Centaur_Headless
    PRODUCT_NAME "chow_centaur"
)

juce_generate_juce_header(Centaur_Headless)
add_custom_command(TARGET Centaur_Headless
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo "copying $<TARGET_FILE:Centaur_Headless> to ${PROJECT_BINARY_DIR}/ChowCentaurHeadless"
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:Centaur_Headless> ${PROJECT_BINARY_DIR}/ChowCentaurHeadless)


target_sources(Centaur_Headless PRIVATE
    main.cpp
)

target_compile_definitions(Centaur_Headless PUBLIC
    JUCE_DISPLAY_SPLASH_SCREEN=0
    JUCE_REPORT_APP_USAGE=0
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
    JucePlugin_Manufacturer="chowdsp"
    JucePlugin_VersionString="${CMAKE_PROJECT_VERSION}"
    JucePlugin_Name="${CMAKE_PROJECT_NAME}"
)

target_include_directories(Centaur_Headless PUBLIC modules)
target_include_directories(Centaur_Headless PUBLIC ../)

target_link_libraries(Centaur_Headless PRIVATE
    BinaryData
    juce::juce_audio_utils
    foleys_gui_magic
    chowdsp_utils
    ChowCentaur
)
