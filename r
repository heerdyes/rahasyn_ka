#!/usr/bin/env bash
set -euo pipefail

clear
#make clean

mkdir -pv bin/data/
cp -v rhka/*.rhka bin/data/

make
make RunRelease

