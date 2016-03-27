#!/bin/bash
# vim: set sts=4 sw=4 et : 

NINJA=$(type -P ninja-build)
if [[ -z $NINJA ]]; then
    NINJA=$(type -P ninja)
    if [[ -z $NINJA ]]; then
        echo "Please install the 'ninja' build system"
        exit 1
    fi
fi

MESON=$(type -P meson)
if [[ -z $MESON ]]; then
    [[ -f meson/meson.py ]] || git submodule init
    git submodule update
    MESON="$PWD/meson/meson.py"
fi

fetch_video() {
    FETCH="$(type -P wget)"
    if [[ -z $FETCH ]]; then
        FETCH="$(type -P curl)"
        if [[ -z $FETCH ]]; then
            echo "Neither curl nor wget were found. Please download $1 to $PWD"
            return
        fi
        FETCH="curl -O -J -L"
    else
        FETCH="wget -c --content-disposition --trust-server-names"
    fi
    $FETCH "$1"
}

mkdir -p build
cd build
"$MESON" ..
"$NINJA"
fetch_video "http://nirbheek.in/files/honda-civic-r/1.mp4"
fetch_video "http://nirbheek.in/files/honda-civic-r/2.mp4"
