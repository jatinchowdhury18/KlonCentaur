#!/bin/bash

set -e
TEAM_ID=$(more ~/Developer/mac_id)

cmake -Bbuild-ios -GXcode -DCMAKE_SYSTEM_NAME=iOS \
    -DCMAKE_OSX_DEPLOYMENT_TARGET=11.4 \
    -DCMAKE_XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY="1,2" \
    -DCMAKE_XCODE_ATTRIBUTE_DEVELOPMENT_TEAM="$TEAM_ID" \
    -DCMAKE_XCODE_ATTRIBUTE_ENABLE_BITCODE="NO" \
    -DCMAKE_XCODE_ATTRIBUTE_SKIP_INSTALL="NO"
    
# cmake --build build-ios --config Release --parallel 8 | xcpretty

# After running this:
# 1. Run build command, with target Standalone, any iOS device, release mode
# 2. Set "Installation Directory" to $(LOCAL_APPS_DIR), for both Standalone and AUv3
# 3. Set "Skip Install" to NO for Standalone, YES for AUv3
# 4. Standalone: manually manage signing, ChowCentaurRelease profile
# 3. Run Archive command

