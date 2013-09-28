project(d2tm)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})
#find_package(SFML 2 REQUIRED system window graphics network audio)
#target_link_libraries(${EXECUTABLE_NAME} ${SFML_LIBRARIES})

# Find SFML
find_package(SFML 2 COMPONENTS graphics window system)

# If found, include and link; otherwise output error message
if(SFML_FOUND)
    include_directories(${SFML_INCLUDE_DIR})
else()
    set(SFML_ROOT "" CACHE PATH "SFML top-level directory")
    message("\n-> SFML directory not found. Set SFML_ROOT to SFML's top-level path (containing \"include\" and \"lib\" directories).")
    message("-> Make sure the SFML libraries with the same configuration (Release/Debug, Static/Dynamic) exist.\n")
endif()

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
)
set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -g")
add_executable(${PROJECT_NAME} ${d2_SRCS})


# Link SFML
target_link_libraries(${PROJECT_NAME} ${SFML_LIBRARIES})
