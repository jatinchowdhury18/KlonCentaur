juce_add_plugin(ChowCentaur
    COMPANY_NAME chowdsp
    PLUGIN_MANUFACTURER_CODE Chow
    PLUGIN_CODE Ctr1
    FORMATS AU VST3 Standalone # VST
    ProductName "ChowCentaur"
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

juce_add_binary_data(BinaryData SOURCES
    gui.xml
    ../GainStageTraining/models/centaur.json
    ../GainStageTraining/models/centaur_small.json
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
    FOLEYS_SHOW_GUI_EDITOR_PALLETTE=0
    FOLEYS_ENABLE_BINARY_DATA=1
)

target_link_libraries(ChowCentaur PRIVATE
    juce::juce_audio_utils
    foleys_gui_magic
    BinaryData
    SharedCode
)
