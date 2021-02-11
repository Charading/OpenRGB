#!/bin/bash

if [ -n $1 ] && [ "$1" = "x86_64" ]
then
ARCHPATH=/usr/x86_64-w64-mingw32/
else
ARCHPATH=/usr/i686-w64-mingw32/
fi
DLLPATH=${ARCHPATH}/bin/
PLUGINPATH=${ARCHPATH}/lib/qt/plugins/

# Deploy MinGW's general libs, Qt and it's dependencies

DLLS=(
    "Qt5Core.dll"
    "Qt5Gui.dll"
    "Qt5Widgets.dll"

    "libbz2-1.dll"
#    "libbz2.dll"
    "libfreetype-6.dll"
    "libgcc_s_dw2-1.dll"
#    "libgcc_s_seh-1.dll"
    "libglib-2.0-0.dll"
    "libgraphite2.dll"
    "libharfbuzz-0.dll"
    "libiconv-2.dll"
    "libintl-8.dll"
    "libpcre-1.dll"
    "libpcre2-16-0.dll"
    "libpng16-16.dll"
    "libssp-0.dll"
    "libstdc++-6.dll"
    "libwinpthread-1.dll"
    "zlib1.dll"
    )

PLUGINS=(
    "platforms"
    "platformthemes"
    "styles"
    #"bearer"
    #"imageformats"
    )

for fn in ${DLLS[@]}; do
    echo "Deploying DLL: ${fn}"
    cp $DLLPATH/${fn} ./${fn}
done

mkdir plugins

for fn in ${PLUGINS[@]}; do
    echo "Deploying Plugin: ${fn}"
    cp -r $PLUGINPATH/${fn} ./
done
