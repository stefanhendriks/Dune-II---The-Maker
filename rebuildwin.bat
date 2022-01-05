rm -rf build
mkdir build
cd build
cmake .. -G "MinGW Makefiles"
cmake --build . --target all -- -j 6
cd ..

