rm -rf Release
mkdir Release
cd Release
cmake -DCMAKE_BUILD_TYPE=Release .. -G "MinGW Makefiles"
cmake --build . --target all -- -j 6
cd ..


