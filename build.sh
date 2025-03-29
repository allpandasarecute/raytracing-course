#!/usr/bin/env bash

SCRIPTPATH=$(dirname "$(realpath "$0")")
# BUILDPATH="$SCRIPTPATH/build"
# mkdir -p $BUILDPATH
# cmake -S $SCRIPTPATH -B $BUILDPATH -DCMAKE_BUILD_TYPE=Release 
# cmake --build $BUILDPATH
odin build "$SCRIPTPATH/odin" -o:speed
