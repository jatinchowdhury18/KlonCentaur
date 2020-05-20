#!/bin/bash

# exit if any command fails
set -e

# update submodules
git submodule update --init --recursive

# set up FRUT
if [ -d modules/FRUT/prefix/FRUT/ ]; then
    echo "FRUT already installed. To do a fresh install, delete modules/FRUT/prefix/, and run this script again."
    exit
fi

cd modules/FRUT/
rm -Rf build/
mkdir build && cd build/
cmake .. -DCMAKE_INSTALL_PREFIX=../prefix -DJUCE_ROOT=../../JUCE
cmake --build . --target install
