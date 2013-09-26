project(dune2)
cmake_minimum_required(VERSION 2.8)

set(d2_SRCS
    d2tm.cpp
    game.cpp
    map.cpp
    cell.cpp
    ini.cpp
    INIReader.cpp
    maploader.cpp
)
set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -g")
add_executable(${PROJECT_NAME} ${d2_SRCS})
target_link_libraries(${PROJECT_NAME} /usr/local/lib/libsfml-graphics.so.2)
target_link_libraries(${PROJECT_NAME} /usr/local/lib/libsfml-window.so.2)
target_link_libraries(${PROJECT_NAME} /usr/local/lib/libsfml-system.so.2)
