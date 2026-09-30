rm -rf Release
mkdir Release
cd Release
cmake -DCMAKE_BUILD_TYPE=Release .. -G "MinGW Makefiles" -DCMAKE_PREFIX_PATH=../deps
cmake --build . --target all -- -j 6
ctest --output-on-failure
cd ..


