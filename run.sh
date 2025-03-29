#!/usr/bin/env bash

SCRIPTPATH=$(dirname "$(realpath "$0")")
# BUILDPATH="$SCRIPTPATH/build"
# $BUILDPATH/pr2 $1 $2
$SCRIPTPATH/odin.bin $1 $2
