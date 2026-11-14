#!/bin/bash

# builds .deb package
A=../../../build/ChowCentaur/ChowCentaur_artefacts/Release
P=ChowCentaur-Linux-amd64-1.4.0

mkdir -p $P/DEBIAN $P/usr/lib/vst3 $P/usr/lib/lv2 $P/usr/bin
cp -r $A/VST3/ChowCentaur.vst3 $P/usr/lib/vst3/
cp -r $A/LV2/ChowCentaur.lv2 $P/usr/lib/lv2/
install -m755 $A/Standalone/ChowCentaur $P/usr/bin/chowcentaur

cat > $P/DEBIAN/control <<EOF
Package: chowcentaur
Version: 1.4.0
Architecture: amd64
Maintainer: cedricus
Description: ChowCentaur (Klon Centaur emulation) - VST3, LV2 and Standalone
EOF

dpkg-deb --root-owner-group --build $P
