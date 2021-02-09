cmake_minimum_required(VERSION 3.15)
set(CMAKE_OSX_DEPLOYMENT_TARGET "10.12" CACHE STRING "Minimum OS X deployment target")
set(CMAKE_CXX_STANDARD 17)
project(ChowCentaur VERSION 1.2.3)

# Using RTNeural with XSimd backend
set(RTNEURAL_XSIMD ON CACHE BOOL "Use RTnNeural with XSimd backend" FORCE)

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
# include_directories(C:/SDKs/asiosdk2.3/common)

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

# build benchmarks
option(BUILD_CENTAUR_BENCH "Builds benchmarking tool for ChowCentaur" OFF)
if(BUILD_CENTAUR_BENCH)
    message("Building Centaur Bench...")
    add_subdirectory(CentaurBench)
endif()
