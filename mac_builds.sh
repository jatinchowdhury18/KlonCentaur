#!/bin/bash

set -e

# clean up old builds
rm -Rf build/
rm -Rf bin/*Mac*

# set up build VST
VST_PATH=~/Developer/VST2_SDK/
sed -i '' "15s~.*~juce_set_vst2_sdk_path(${VST_PATH})~" CMakeLists.txt
sed -i '' '5s/#//' ChowCentaur/CMakeLists.txt

# cmake new builds
TEAM_ID=$(more ~/Developer/mac_id)
cmake -Bbuild -GXcode -DCMAKE_XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY="Apple Distribution" \
    -DCMAKE_XCODE_ATTRIBUTE_DEVELOPMENT_TEAM=$TEAM_ID \
    -DCMAKE_XCODE_ATTRIBUTE_CODE_SIGN_STYLE="Manual" \
    -D"CMAKE_OSX_ARCHITECTURES=arm64;x86_64"
cmake --build build --config Release -j8 | xcpretty

# copy builds to bin
mkdir -p bin/Mac
declare -a plugins=("ChowCentaur")
for plugin in "${plugins[@]}"; do
    cp -R build/${plugin}/${plugin}_artefacts/Release/Standalone/${plugin}.app bin/Mac/${plugin}.app
    cp -R build/${plugin}/${plugin}_artefacts/Release/VST/${plugin}.vst bin/Mac/${plugin}.vst
    cp -R build/${plugin}/${plugin}_artefacts/Release/VST3/${plugin}.vst3 bin/Mac/${plugin}.vst3
    cp -R build/${plugin}/${plugin}_artefacts/Release/AU/${plugin}.component bin/Mac/${plugin}.component
done

# reset CMakeLists.txt
git restore CMakeLists.txt
git restore ChowCentaur/CMakeLists.txt

# zip builds
VERSION=$(cut -f 2 -d '=' <<< "$(grep 'CMAKE_PROJECT_VERSION:STATIC' build/CMakeCache.txt)")
(
    cd bin
    rm -f "ChowCentaur-Mac-${VERSION}.zip"
    zip -r "ChowCentaur-Mac-${VERSION}.zip" Mac
)
