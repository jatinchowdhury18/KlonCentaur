#!/bin/bash

set -e

build64(){
    cmake -Bbuild -G"Visual Studio 15 2017 Win64"
    cmake --build build --config Release -j4
}

build32(){
    cmake -Bbuild32 -G"Visual Studio 15 2017"
    cmake --build build32 --config Release -j4
}

# clean up old builds
rm -Rf build/
rm -Rf build32/
rm -Rf bin/*Win64*
rm -Rf bin/*Win32*

# set up VST and ASIO paths
sed -i -e "19s/#//" CMakeLists.txt
sed -i -e "20s/#//" CMakeLists.txt
sed -i -e '5s/#//' ChowCentaur/CMakeLists.txt
sed -i -e '42s/#//' ChowCentaur/CMakeLists.txt

# cmake new builds
build64 &
build32 &
wait

# copy builds to bin
mkdir -p bin/Win64
mkdir -p bin/Win32
declare -a plugins=("ChowCentaur")
for plugin in "${plugins[@]}"; do
    cp -R build/${plugin}/${plugin}_artefacts/Release/Standalone/${plugin}.exe bin/Win64/${plugin}.exe
    cp -R build/${plugin}/${plugin}_artefacts/Release/VST/${plugin}.dll bin/Win64/${plugin}.dll
    cp -R build/${plugin}/${plugin}_artefacts/Release/VST3/${plugin}.vst3 bin/Win64/${plugin}.vst3

    cp -R build32/${plugin}/${plugin}_artefacts/Release/Standalone/${plugin}.exe bin/Win32/${plugin}.exe
    cp -R build32/${plugin}/${plugin}_artefacts/Release/VST/${plugin}.dll bin/Win32/${plugin}.dll
    cp -R build32/${plugin}/${plugin}_artefacts/Release/VST3/${plugin}.vst3 bin/Win32/${plugin}.vst3
done

# reset CMakeLists.txt
git restore CMakeLists.txt
git restore ChowCentaur/CMakeLists.txt

# zip builds
VERSION=$(cut -f 2 -d '=' <<< "$(grep 'CMAKE_PROJECT_VERSION:STATIC' build/CMakeCache.txt)")
(
    cd bin
    rm -f "ChowCentaur-Win64-${VERSION}.zip"
    rm -f "ChowCentaur-Win32-${VERSION}.zip"
    zip -r "ChowCentaur-Win64-${VERSION}.zip" Win64
    zip -r "ChowCentaur-Win32-${VERSION}.zip" Win32
)
