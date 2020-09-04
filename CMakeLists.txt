cmake_minimum_required(VERSION 3.15)
project(ChowCentaur VERSION 1.2.0)

set(CMAKE_CXX_STANDARD 17)
add_subdirectory(modules)
include_directories(modules/JUCE/modules)
include_directories(modules/foleys_gui_magic)
include_directories(modules/WaveDigitalFilters/shared)
include_directories(modules/Eigen)

# comment to use STL implementation instead of Eigen
add_definitions(-DUSE_EIGEN)

# juce_set_vst2_sdk_path(C:/SDKs/VST_SDK/VST2_SDK/)

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
