#!/bin/bash

set -e

sudo apt update
sudo apt install -y build-essential cmake git pkg-config \
  libasound2-dev libjack-jackd2-dev libcurl4-openssl-dev \
  libfreetype-dev libfontconfig1-dev libx11-dev libxcomposite-dev \
  libxcursor-dev libxext-dev libxinerama-dev libxrandr-dev \
  libxrender-dev libglu1-mesa-dev mesa-common-dev \
  libwebkit2gtk-4.1-dev libgtk-3-dev lv2-dev

# create a link from 4.0 to 4.1 to patch deprecated
mkdir -p ~/.local/share/pkgconfig
cat > ~/.local/share/pkgconfig/webkit2gtk-4.0.pc <<'EOF'
Name: webkit2gtk-4.0
Description: shim vers webkit2gtk-4.1
Version: 2.40.0
Requires: webkit2gtk-4.1
EOF

export PKG_CONFIG_PATH=$HOME/.local/share/pkgconfig:$PKG_CONFIG_PATH

# careate a link to patch deprecated lv2
mkdir -p ~/lv2shim/lv2/lv2plug.in/ns/ext ~/lv2shim/lv2/lv2plug.in/ns/extensions
ln -sfn /usr/include/lv2/core ~/lv2shim/lv2/lv2plug.in/ns/lv2core
for d in /usr/include/lv2/*/; do
  n=$(basename "$d")
  ln -sfn "$d" ~/lv2shim/lv2/lv2plug.in/ns/ext/$n
done
ln -sfn /usr/include/lv2/ui ~/lv2shim/lv2/lv2plug.in/ns/extensions/ui

git submodule update --init --recursive

cmake -Bbuild -DCMAKE_BUILD_TYPE=Release -Wno-dev -DCMAKE_CXX_FLAGS="-I$HOME/lv2shim"

# cmake new builds
A=build/ChowCentaur/ChowCentaur_artefacts/Release

for T in ChowCentaur_VST3 ChowCentaur_LV2 ChowCentaur_Standalone; do
  echo "=== Build $T ==="
  cmake --build build --target $T -j"$(nproc)" 2>&1 | grep -iE "error|fatal|Built target" || true
done

# checking: script stop if a piece is missing
test -f $A/VST3/ChowCentaur.vst3/Contents/x86_64-linux/ChowCentaur.so
test -f $A/LV2/ChowCentaur.lv2/ChowCentaur.so
test -x $A/Standalone/ChowCentaur

# create installer
echo "Creating installer..."
cd Installers/linux/debian/
bash build_debian_installer.sh

