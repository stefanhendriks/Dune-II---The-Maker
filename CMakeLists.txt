project(d2tm)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})
find_package(SFML 2 REQUIRED graphics window system)
include_directories(${SFML_INCLUDE_DIR})

set(d2_SRCS
    d2tm.cpp
    game.cpp
    map.cpp
    cell.cpp
    ini.cpp
    INIReader.cpp
    maploader.cpp
    unit_move_behavior.cpp
    unit.cpp
    selectionbox.cpp
    mouse.cpp
)

set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -g")
add_executable(${PROJECT_NAME} ${d2_SRCS})

# Link SFML
target_link_libraries(${PROJECT_NAME} ${SFML_LIBRARIES})
