#!/bin/bash
set -euo pipefail

rm -f bin/d2tm
mkdir -p build
mkdir -p bin

cd build
cmake ..
cmake --build . -j 8
ctest --output-on-failure
cd ..
cp build/d2tm bin
