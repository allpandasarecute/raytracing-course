#!/usr/bin/env bash

SCRIPT=$(realpath "$0")
SCRIPTPATH=$(dirname "$SCRIPT")
mkdir -p $SCRIPTPATH/build
cmake -S $SCRIPTPATH -B $SCRIPTPATH/build 
cmake --build $SCRIPTPATH/build 
