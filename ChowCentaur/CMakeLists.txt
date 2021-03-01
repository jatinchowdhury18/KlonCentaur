if(IOS)
    juce_add_plugin(ChowCentaur
        COMPANY_NAME chowdsp
        PLUGIN_MANUFACTURER_CODE Chow
        PLUGIN_CODE Ctr1
        FORMATS Standalone AUv3
        ProductName "ChowCentaur"
        ICON_BIG res/logo.png
        MICROPHONE_PERMISSION_ENABLED TRUE
    )
else()
    juce_add_plugin(ChowCentaur
        COMPANY_NAME chowdsp
        PLUGIN_MANUFACTURER_CODE Chow
        PLUGIN_CODE Ctr1
        FORMATS AU VST3 Standalone LV2 #VST
        ProductName "ChowCentaur"
        ICON_BIG res/logo.png
        LV2_URI https://github.com/jatinchowdhury18/KlonCentaur
        MICROPHONE_PERMISSION_ENABLED TRUE
    )
endif()

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
    res/gui.xml
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
    juce::juce_audio_plugin_client
    foleys_gui_magic
    chowdsp_utils
    BinaryData
    SharedCode
)

# we need these flags for notarization on MacOS
option(MACOS_RELEASE "Set build flags for MacOS Release" OFF)
if(MACOS_RELEASE)
    message(STATUS "Setting MacOS release flags...")
    set_target_properties(ChowCentaur_Standalone PROPERTIES
        XCODE_ATTRIBUTE_ENABLE_HARDENED_RUNTIME YES)
endif()

if(IOS)
    message(STATUS "Setting iOS-specific properties...")

    foreach(target IN ITEMS BinaryData RTNeural SharedCode ChowCentaur ChowCentaur_Standalone ChowCentaur_AUv3)
        set_target_properties(${target}
            PROPERTIES
                RUNTIME_OUTPUT_DIRECTORY "./"
                ARCHIVE_OUTPUT_DIRECTORY "./"
                LIBRARY_OUTPUT_DIRECTORY "./")
    endforeach()

    set_target_properties(ChowCentaur_Standalone PROPERTIES
            XCODE_ATTRIBUTE_INSTALL_PATH "$(LOCAL_APPS_DIR)"
            XCODE_ATTRIBUTE_SKIP_INSTALL "NO")

    set_target_properties(ChowCentaur_AUv3 PROPERTIES
            XCODE_ATTRIBUTE_INSTALL_PATH "$(LOCAL_APPS_DIR)/ChowCentaur.app/PlugIns"
            XCODE_ATTRIBUTE_SKIP_INSTALL "NO")
endif()

# build headless
option(BUILD_CENTAUR_HEADLESS "Builds headless tool for ChowCentaur" OFF)
if(BUILD_CENTAUR_HEADLESS)
    message(STATUS "Building Centaur Headless...")
    add_subdirectory(headless)
endif()
