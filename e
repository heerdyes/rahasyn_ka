#!/usr/bin/env bash
set -euo pipefail

gedit &
sleep 4

gedit src/ofApp.cpp
gedit src/ofApp.h
gedit src/synth.h
gedit src/ftbl.h
gedit src/fsm.h
gedit src/uts.h
gedit src/hwif.h

