rm -rf Release || exit /b 1
mkdir Release || exit /b 1
cd Release || exit /b 1
cmake -DCMAKE_BUILD_TYPE=Release .. -G "MinGW Makefiles" || exit /b 1
cmake --build . --target all -- -j 6 || exit /b 1
ctest --output-on-failure || exit /b 1
cd .. || exit /b 1
