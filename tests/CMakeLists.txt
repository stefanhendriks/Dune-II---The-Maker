include(FetchContent)

# The root CMakeLists.txt sets CMAKE_CXX_COMPILER / CMAKE_C_COMPILER after its own
# project() call. CMake's initial compiler detection writes the system default (e.g.
# /usr/bin/c++) into the cache. The root then overwrites the variable (e.g. to
# /opt/homebrew/bin/g++-15 on arm64, or "clang++" on Intel). When FetchContent brings
# in Catch2, that sub-project calls project() which re-detects the compiler from the
# cache and may restore the system default, causing CMake to detect a "variables changed"
# mismatch and re-run configure indefinitely (infinite loop on Apple Silicon).
#
# Fix: before FetchContent runs, resolve any bare names to absolute paths and then
# unconditionally force-update the cache to match the current variable value. This
# ensures Catch2's project() call sees the intended compiler and does not re-detect.
foreach(_lang CXX C)
    set(_compiler_var "CMAKE_${_lang}_COMPILER")
    if("${${_compiler_var}}" STREQUAL "")
        continue()
    endif()

    # Resolve bare names to absolute paths (needed on Apple Intel where "clang++"
    # is set instead of a full path).
    if(NOT IS_ABSOLUTE "${${_compiler_var}}")
        find_program(_compiler_abs "${${_compiler_var}}"
            PATHS /usr/bin /usr/local/bin /opt/homebrew/bin
            NO_DEFAULT_PATH)
        if(NOT _compiler_abs)
            find_program(_compiler_abs "${${_compiler_var}}")
        endif()
        if(_compiler_abs)
            set(${_compiler_var} "${_compiler_abs}")
        endif()
        unset(_compiler_abs CACHE)
    endif()

    # Force the cache to match the current value. Without this, Catch2's project()
    # call re-detects the compiler and may write a different value, which triggers
    # an infinite configure loop (observed on Apple Silicon / arm64).
    set(${_compiler_var} "${${_compiler_var}}" CACHE FILEPATH "${_lang} compiler" FORCE)
endforeach()
unset(_lang)
unset(_compiler_var)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.8.1
)
FetchContent_MakeAvailable(Catch2)

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

add_executable(d2tm_tests
    map/test_MapGeometry.cpp
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
