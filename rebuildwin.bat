rm -rf build || exit /b 1
mkdir build || exit /b 1
cd build || exit /b 1
cmake .. -G "MinGW Makefiles" || exit /b 1
cmake --build . --target all -- -j 6 || exit /b 1
ctest --output-on-failure || exit /b 1
cd .. || exit /b 1

copy build\d2tm.exe bin\ || exit /b 1
