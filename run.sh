#!/usr/bin/env bash

SCRIPTPATH=$(dirname "$(realpath "$0")")
$SCRIPTPATH/pr2 $1 $2
