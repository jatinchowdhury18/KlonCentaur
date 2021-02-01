juce_add_plugin(ChowCentaur
    COMPANY_NAME chowdsp
    PLUGIN_MANUFACTURER_CODE Chow
    PLUGIN_CODE Ctr1
    FORMATS AU VST3 Standalone AUv3 # VST
    ProductName "ChowCentaur"
    ICON_BIG res/logo.png
)

add_dependencies(SharedCode ChowCentaur)

juce_generate_juce_header(ChowCentaur)

target_sources(ChowCentaur PRIVATE
    ChowCentaurPlugin.h
    ChowCentaurPlugin.cpp
    GainStageProc.h
    GainStageProc.cpp
    GainStageMLProc.h
    GainStageMLProc.cpp
)

add_subdirectory(gui)

juce_add_binary_data(BinaryData SOURCES
    res/gui.xml
    res/knob.svg
    res/pointer.svg
    res/RobotoCondensed-Bold.ttf
    res/RobotoCondensed-Regular.ttf
    ../GainStageTraining/models/centaur_0.json
    ../GainStageTraining/models/centaur_25.json
    ../GainStageTraining/models/centaur_50.json
    ../GainStageTraining/models/centaur_75.json
    ../GainStageTraining/models/centaur_100.json
)

# Need to build BinaryData with -fPIC flag on Linux
set_target_properties(BinaryData PROPERTIES
    POSITION_INDEPENDENT_CODE TRUE)

target_compile_definitions(ChowCentaur PUBLIC
    JUCE_DISPLAY_SPLASH_SCREEN=0
    JUCE_REPORT_APP_USAGE=0
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
    # JUCE_ASIO=1
    FOLEYS_SHOW_GUI_EDITOR_PALLETTE=0
    FOLEYS_ENABLE_BINARY_DATA=1
)

target_link_libraries(ChowCentaur PRIVATE
    juce::juce_audio_utils
    foleys_gui_magic
    BinaryData
    SharedCode
)
