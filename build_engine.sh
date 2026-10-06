#!/usr/bin/env bash
set -e
cd engine

BUILD_DIR="build"
mkdir -p "$BUILD_DIR"

cmake -S . -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=Release
cmake --build "$BUILD_DIR" --target mockfish -- -j"$(nproc)"
