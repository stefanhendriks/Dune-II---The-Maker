find_package(Catch2 3 REQUIRED)

# Test files
set(TEST_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/map/test_MapGeometry.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/test_cExpandingRectangle.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/test_cIniFile.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/test_cIniUtils.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/test_cPoint.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/test_cRectangle.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/test_d2tm_math.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/test_SfxTrackAllocator.cpp
)

# Production source files the tests depend on.
# No SDL runtime linkage required — SDL headers are needed only to compile
# types like cRectangle (which declares an inline toSDL() helper using SDL_Rect).
set(TEST_SOURCES
    ${CMAKE_SOURCE_DIR}/src/gameobjects/map/MapGeometry.cpp
    ${CMAKE_SOURCE_DIR}/src/utils/RNG.cpp
    ${CMAKE_SOURCE_DIR}/src/utils/d2tm_math.cpp
    ${CMAKE_SOURCE_DIR}/src/utils/cPoint.cpp
    ${CMAKE_SOURCE_DIR}/src/utils/cRectangle.cpp
    ${CMAKE_SOURCE_DIR}/src/utils/cExpandingRectangle.cpp
    ${CMAKE_SOURCE_DIR}/src/utils/cIniFile.cpp
    ${CMAKE_SOURCE_DIR}/src/utils/Log.cpp
    ${CMAKE_SOURCE_DIR}/src/utils/SfxTrackAllocator.cpp
)

add_executable(d2tm_tests
    ${TEST_FILES}
    ${TEST_SOURCES}
)

# SDL3_INCLUDE_DIRS is populated by pkg-config in the root CMakeLists.txt on macOS.
# On Linux/Windows it is empty: the SDL3 targets from link_libraries() provide the includes.
target_include_directories(d2tm_tests
    PRIVATE
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/src/include
        ${SDL3_INCLUDE_DIRS}
)

target_link_libraries(d2tm_tests PRIVATE Catch2::Catch2WithMain)
target_link_libraries(d2tm_tests PRIVATE cpp_stacktrace)

include(CTest)
include(Catch)
catch_discover_tests(d2tm_tests)
