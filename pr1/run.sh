#!/usr/bin/env bash

SCRIPT=$(realpath "$0")
SCRIPTPATH=$(dirname "$SCRIPT")
$SCRIPTPATH/build/pr1 $1 $2
