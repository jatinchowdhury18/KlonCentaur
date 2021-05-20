cmake_minimum_required(VERSION 3.15)
set(CMAKE_OSX_DEPLOYMENT_TARGET "10.12" CACHE STRING "Minimum OS X deployment target")
set(CMAKE_CXX_STANDARD 17)
project(ChowCentaur VERSION 1.3.7)

# Using RTNeural with XSimd backend
set(RTNEURAL_XSIMD ON CACHE BOOL "Use RTNeural with STL backend" FORCE)
add_definitions(-DUSE_XSIMD=1)

add_subdirectory(modules)

# juce_set_vst2_sdk_path(C:/SDKs/VST_SDK/VST2_SDK/)
# include_directories(ASIO_SDK)

# build main plugin
add_subdirectory(ChowCentaur)
