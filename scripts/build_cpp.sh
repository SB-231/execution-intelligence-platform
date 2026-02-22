#!/usr/bin/env bash
set -euo pipefail
mkdir -p cpp_engine/build
cmake -S cpp_engine -B cpp_engine/build
cmake --build cpp_engine/build -j