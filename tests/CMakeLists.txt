include(FetchContent)

# On Apple Intel macs, the root CMakeLists.txt sets CMAKE_CXX_COMPILER / CMAKE_C_COMPILER
# to bare names (e.g. "clang++") after its own project() call.  When FetchContent brings
# in Catch2, that sub-project calls project() which requires an absolute compiler path.
#
# Fix: resolve bare names to absolute paths in this scope so child scopes (FetchContent)
# inherit the absolute path. IS_ABSOLUTE is a no-op when the path is already absolute
# (arm64 uses /opt/homebrew/bin/g++-15; Linux/Windows are detected with absolute paths).
foreach(_lang CXX C)
    set(_compiler_var "CMAKE_${_lang}_COMPILER")
    if(NOT IS_ABSOLUTE "${${_compiler_var}}")
        find_program(_compiler_abs "${${_compiler_var}}"
            PATHS /usr/bin /usr/local/bin /opt/homebrew/bin
            NO_DEFAULT_PATH)
        if(NOT _compiler_abs)
            # Fallback: let find_program search PATH normally
            find_program(_compiler_abs "${${_compiler_var}}")
        endif()
        if(_compiler_abs)
            # Set normal variable in this scope — this shadows the parent's bare name
            # and is inherited by any add_subdirectory() called from here (FetchContent).
            set(${_compiler_var} "${_compiler_abs}")
            # Also update the cache so subsequent cmake runs see the absolute path.
            set(${_compiler_var} "${_compiler_abs}" CACHE FILEPATH "${_lang} compiler" FORCE)
        endif()
        unset(_compiler_abs CACHE)
    endif()
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
