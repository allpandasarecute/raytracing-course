#!/usr/bin/env bash

SCRIPTPATH=$(dirname "$(realpath "$0")")
BUILDPATH="$SCRIPTPATH/build"
$BUILDPATH/pr1 $1 $2
