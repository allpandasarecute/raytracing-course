#!/usr/bin/env bash

SCRIPTPATH=$(dirname "$(realpath "$0")")
/usr/local/odin/odin build "$SCRIPTPATH/odin" -o:speed -out:pr5
# odin build "$SCRIPTPATH/odin" -o:speed -out:pr5
