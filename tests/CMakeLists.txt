find_package(Catch2 3 REQUIRED)

# Only the source files that MapGeometry directly or transitively depends on.
# No SDL runtime linkage required — SDL2 headers are needed only to compile
# cRectangle.h (which declares an inline toSDL() helper using SDL_Rect).
set(TEST_SOURCES
    ${CMAKE_SOURCE_DIR}/src/map/MapGeometry.cpp
    ${CMAKE_SOURCE_DIR}/src/utils/RNG.cpp
    ${CMAKE_SOURCE_DIR}/src/utils/d2tm_math.cpp
    ${CMAKE_SOURCE_DIR}/src/utils/cPoint.cpp
    ${CMAKE_SOURCE_DIR}/src/utils/cRectangle.cpp
)

file(GLOB_RECURSE AUTO_TEST_FILES CONFIGURE_DEPENDS "*.cpp")

add_executable(d2tm_tests
    ${AUTO_TEST_FILES}
    ${TEST_SOURCES}
)

# SDL2_INCLUDE_DIRS is populated by the root CMakeLists.txt for all platforms
# (pkg-config on macOS, find_package on Linux/Windows).
target_include_directories(d2tm_tests
    PRIVATE
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/src/include
        ${SDL2_INCLUDE_DIRS}
)

target_link_libraries(d2tm_tests PRIVATE Catch2::Catch2WithMain)

include(CTest)
include(Catch)
catch_discover_tests(d2tm_tests)
