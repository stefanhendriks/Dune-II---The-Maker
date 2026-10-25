#!/bin/bash
set -euo pipefail

rm -f bin/d2tm
mkdir -p build

cd build
cmake ..
cmake --build . -j 8
ctest --output-on-failure
cd ..
cp build/d2tm bin
