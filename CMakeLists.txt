cmake_minimum_required(VERSION 3.15)
set(CMAKE_OSX_DEPLOYMENT_TARGET "10.12" CACHE STRING "Minimum OS X deployment target")
set(CMAKE_CXX_STANDARD 17)
project(ChowCentaur VERSION 1.3.6)

# Using RTNeural with XSimd backend
if(APPLE)
    set(RTNEURAL_ACCELERATE ON CACHE BOOL "Use RTNeural with Accelerate backend" FORCE)
else()
    set(RTNEURAL_XSIMD ON CACHE BOOL "Use RTNeural with XSimd backend" FORCE)
endif()

add_subdirectory(modules)
if(UNIX AND NOT APPLE)
    include_directories(modules/distrho-juce/modules)
else()
    include_directories(modules/JUCE/modules)
endif()
include_directories(modules/foleys_gui_magic)
include_directories(modules/chowdsp_utils)
include_directories(modules/RTNeural)

# juce_set_vst2_sdk_path(C:/SDKs/VST_SDK/VST2_SDK/)
# include_directories(ASIO_SDK)

# build shared code as static lib
add_subdirectory(SharedCode)
include_directories(SharedCode)

# build main plugin
add_subdirectory(ChowCentaur)

# build sub-circuits
option(BUILD_SUBCIRCUITS "Builds ChowCentaur subcircuits" OFF)
if(BUILD_SUBCIRCUITS)
    message("Building SubCircuits...")
    add_subdirectory(SubCircuits)
endif()
