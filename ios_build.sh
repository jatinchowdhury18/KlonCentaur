#!/bin/bash

set -e
TEAM_ID=$(more ~/Developer/mac_id)

cmake -Bbuild-ios -GXcode -DCMAKE_SYSTEM_NAME=iOS \
    -DCMAKE_OSX_DEPLOYMENT_TARGET=11.4 \
    -DCMAKE_XCODE_ATTRIBUTE_DEVELOPMENT_TEAM="$TEAM_ID" \
    -DCMAKE_XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY="1,2" \
    -DCMAKE_XCODE_ATTRIBUTE_ENABLE_BITCODE="NO"

xcodebuild -project build-ios/ChowCentaur.xcodeproj \
  -scheme ChowCentaur_Standalone archive -configuration Release \
  -sdk iphoneos -jobs 9 | xcpretty
