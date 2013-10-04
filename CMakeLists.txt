project(d2tm)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})
find_package(SFML 2 REQUIRED graphics window system)
find_package(Thor 2 REQUIRED)
include_directories(${SFML_INCLUDE_DIR})

set(d2_SRCS
    d2tm.cpp
    game.cpp
    map.cpp
    cell.cpp
    ini.cpp
    INIReader.cpp
    maploader.cpp
    unit.cpp
    selectionbox.cpp
    mouse.cpp
    player.cpp
    houses.cpp
    actionmanager.cpp
)

IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set(CMAKE_CXX_FLAGS "-std=c++11 -stdlib=libc++ -Wall -g")
ELSE()
  set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -g")
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

add_executable(${PROJECT_NAME} ${d2_SRCS})

# Link SFML
target_link_libraries(${PROJECT_NAME} ${SFML_LIBRARIES})
target_link_libraries(${PROJECT_NAME} ${THOR_LIBRARY})
