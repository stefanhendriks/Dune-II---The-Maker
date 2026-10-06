rm -rf build
mkdir build
cd build
cmake .. -G "MinGW Makefiles" -DCMAKE_PREFIX_PATH=../deps
cmake --build . --target all -- -j 6
ctest --output-on-failure
cd ..

