rm -rf build
mkdir build
cd build
cmake ..
cmake --build . --target all -- -j 6
ctest --output-on-failure
cd ..

