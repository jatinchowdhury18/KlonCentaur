#!/bin/bash

#!/bin/bash

set -e
TEAM_ID=$(more ~/Developer/mac_id)

if [ "$1" == "help" ]; then
  echo "Run bash ios_build.sh build clean"
  echo "Open project in Xcode and run Product->archive"
  echo "Run bash ios_build.sh version"
  echo "Go to Xcode Archive Organizer and upload!"
  exit
fi

if [ "$1" == "build" ]; then
echo "Running CMake configuration..."

# clean up old builds
if [ "$2" == "clean" ]; then rm -Rf build-ios; fi

# generate new builds
cmake -Bbuild-ios -GXcode -DCMAKE_SYSTEM_NAME=iOS \
    -DCMAKE_OSX_DEPLOYMENT_TARGET=11.4 \
    -DCMAKE_XCODE_ATTRIBUTE_DEVELOPMENT_TEAM="$TEAM_ID" \
    -DCMAKE_XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY="1,2" \
    -DCMAKE_XCODE_ATTRIBUTE_ENABLE_BITCODE="NO"
fi

# xcodebuild -project build-ios/ChowCentaur.xcodeproj \
#   -scheme ChowCentaur_Standalone archive -configuration Release \
#   -sdk iphoneos -jobs 9 -archivePath ChowCentaur.xcarchive | xcpretty

if [ "$1" == "version" ]; then
  echo "Setting version for last built archive"

  # set version number to include commit hash
  parent=~/Library/Developer/Xcode/Archives
  archive_dir=$(ls -rt $parent | tail -n 1)
  archive=$(ls -rt $parent/$archive_dir | tail -n 1)
  PLIST=$parent/$archive_dir/$archive/Info.plist
  COMMIT=$(git log --pretty=format:'%h' -n 1)
  VERSION=$(cut -f 2 -d '=' <<< "$(grep 'CMAKE_PROJECT_VERSION:STATIC' build-ios/CMakeCache.txt)")
  BUILD_NUMBER="$VERSION-$COMMIT"
  /usr/libexec/Plistbuddy -c "Set ApplicationProperties:CFBundleVersion $BUILD_NUMBER" "$PLIST"
fi

# move to archives folder so Xcode can find it
# archive_dir="$HOME/Library/Developer/Xcode/Archives/$(date '+%Y-%m-%d')"
# mkdir -p "$archive_dir"
# mv ChowCentaur.xcarchive "$archive_dir/ChowCentaur-$COMMIT.xcarchive"
