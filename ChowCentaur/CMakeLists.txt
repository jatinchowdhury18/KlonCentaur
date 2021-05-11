# set plugin formats to build
if(IOS)
    set(JUCE_FORMATS Standalone AUv3)
else()
    set(JUCE_FORMATS AU VST3 Standalone)
endif()

# Build LV2 only on Linux
if(UNIX AND NOT APPLE)
    message(STATUS "Building LV2 plugin format")
    list(APPEND JUCE_FORMATS LV2)
endif()

# Build VST2 is SDK set
if(TARGET juce_vst2_sdk)
    message(STATUS "Building VST2 plugin format")
    list(APPEND JUCE_FORMATS VST)
endif()

juce_add_plugin(ChowCentaur
    COMPANY_NAME chowdsp
    PLUGIN_MANUFACTURER_CODE Chow
    PLUGIN_CODE Ctr1
    FORMATS ${JUCE_FORMATS}
    ProductName "ChowCentaur"
    ICON_BIG res/logo.png
    LV2_URI https://github.com/jatinchowdhury18/KlonCentaur
    MICROPHONE_PERMISSION_ENABLED TRUE
)

juce_generate_juce_header(ChowCentaur)
include_directories(../modules/RTNeural)
include_directories(./)

target_sources(ChowCentaur PRIVATE
    ChowCentaurPlugin.h
    ChowCentaurPlugin.cpp
    
    CommonProcessors/InputBufferProcessor.h
    CommonProcessors/InputBufferProcessor.cpp
    CommonProcessors/OutputStageProcessor.h
    CommonProcessors/OutputStageProcessor.cpp
    CommonProcessors/ToneFilterProcessor.h
    CommonProcessors/ToneFilterProcessor.cpp
    
    GainStageProcessors/PreAmpStage.h
    GainStageProcessors/PreAmpStage.cpp
    GainStageProcessors/AmpStage.h
    GainStageProcessors/ClippingStage.h
    GainStageProcessors/ClippingStage.cpp
    GainStageProcessors/FeedForward2.h
    GainStageProcessors/FeedForward2.cpp
    GainStageProcessors/GainStageProc.h
    GainStageProcessors/GainStageProc.cpp
    GainStageProcessors/SummingAmp.h

    GainStageML/GainStageMLProc.h
    GainStageML/GainStageMLProc.cpp
    GainStageML/RNNModel.h
    GainStageML/RNNModel.cpp

    gui/TipJar.cpp
)

juce_add_binary_data(BinaryData SOURCES
    res/gui.xml
    res/gui_ios.xml
    ../GainStageTraining/models/centaur_0.json
    ../GainStageTraining/models/centaur_25.json
    ../GainStageTraining/models/centaur_50.json
    ../GainStageTraining/models/centaur_75.json
    ../GainStageTraining/models/centaur_100.json
)

target_precompile_headers(ChowCentaur PRIVATE pch.h)

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
    juce_plugin_modules
    BinaryData
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

    foreach(target IN ITEMS BinaryData toms917 RTNeural juce_plugin_modules ChowCentaur ChowCentaur_Standalone ChowCentaur_AUv3)
        set_target_properties(${target}
            PROPERTIES
                RUNTIME_OUTPUT_DIRECTORY "./"
                ARCHIVE_OUTPUT_DIRECTORY "./"
                LIBRARY_OUTPUT_DIRECTORY "./")
    endforeach()

    set_target_properties(ChowCentaur_Standalone PROPERTIES
            XCODE_ATTRIBUTE_INSTALL_PATH "$(LOCAL_APPS_DIR)"
            XCODE_ATTRIBUTE_SKIP_INSTALL "NO"
            XCODE_ATTRIBUTE_ENABLE_IN_APP_PURCHASE "YES")

    set_target_properties(ChowCentaur_AUv3 PROPERTIES
            XCODE_ATTRIBUTE_INSTALL_PATH "$(LOCAL_APPS_DIR)/ChowCentaur.app/PlugIns"
            XCODE_ATTRIBUTE_SKIP_INSTALL "NO"
            XCODE_ATTRIBUTE_ENABLE_IN_APP_PURCHASE "YES")
endif()

# build headless
option(BUILD_CENTAUR_HEADLESS "Builds headless tool for ChowCentaur" OFF)
if(BUILD_CENTAUR_HEADLESS)
    message(STATUS "Building Centaur Headless...")
    add_subdirectory(headless)
endif()
