#!/usr/bin/env bash
set -euo pipefail

kate &
sleep 4

kate src/ofApp.cpp
kate src/ofApp.h
kate src/main.cpp

