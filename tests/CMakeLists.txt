find_package(Catch2 3 REQUIRED)

# Test files
set(TEST_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/controls/test_cKeyBindings.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/map/test_MapGeometry.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/map/test_cPreviewMapsSave.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/test_cHandleArgument.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/test_cExpandingRectangle.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/test_cIniFile.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/test_cIniUtils.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/test_IniParsing.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/test_IniScenarioParsing.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/test_CampaignProgress.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/test_cPoint.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/test_cRectangle.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/test_d2tm_math.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/test_SfxTrackAllocator.cpp
)

# Production source files the tests depend on.
# No SDL runtime linkage required — SDL headers are needed only to compile
# types like cRectangle (which declares an inline toSDL() helper using SDL_Rect).
set(TEST_SOURCES
    ${CMAKE_SOURCE_DIR}/src/controls/cKeyBindings.cpp
    ${CMAKE_SOURCE_DIR}/src/utils/cHandleArgument.cpp
    ${CMAKE_SOURCE_DIR}/src/gameobjects/map/MapGeometry.cpp
    ${CMAKE_SOURCE_DIR}/src/gameobjects/map/cPreviewMaps.cpp
    ${CMAKE_SOURCE_DIR}/src/drawers/SDLDrawer.cpp
    ${CMAKE_SOURCE_DIR}/src/utils/RNG.cpp
    ${CMAKE_SOURCE_DIR}/src/utils/d2tm_math.cpp
    ${CMAKE_SOURCE_DIR}/src/utils/cPoint.cpp
    ${CMAKE_SOURCE_DIR}/src/utils/cRectangle.cpp
    ${CMAKE_SOURCE_DIR}/src/utils/cExpandingRectangle.cpp
    ${CMAKE_SOURCE_DIR}/src/utils/cIniFile.cpp
    ${CMAKE_SOURCE_DIR}/src/utils/IniParsing.cpp
    ${CMAKE_SOURCE_DIR}/src/utils/cIniUtils.cpp
    ${CMAKE_SOURCE_DIR}/src/game/CampaignProgress.cpp
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

# cKeyBindings calls SDL scancode functions. On Linux/Windows link_libraries() in the root
# CMakeLists.txt provides SDL3; on macOS (pkg-config) it must be linked explicitly.
if(APPLE)
    target_link_libraries(d2tm_tests PRIVATE ${SDL3_LIBRARIES})
    # Same as the game target: Apple libc++ has no float std::from_chars, IniParsing.cpp needs the fallback.
    target_compile_definitions(d2tm_tests PRIVATE D2TM_CLANG=1)
endif()

# Opt-in coverage instrumentation (GCC/gcov) for the unit tests: `cmake .. -DD2TM_COVERAGE=ON`.
# d2tm_tests and the game itself (root CMakeLists.txt, for the zero baseline) are instrumented, not the seedgen tests. CI: the COVERAGE job in .github/workflows/build_pr.yml.
option(D2TM_COVERAGE "Instrument d2tm_tests and the game with --coverage (gcov)" OFF)
if(D2TM_COVERAGE)
    target_compile_options(d2tm_tests PRIVATE --coverage -O0 -g)
    target_link_options(d2tm_tests PRIVATE --coverage)
endif()

include(CTest)
include(Catch)
catch_discover_tests(d2tm_tests)

# seedgen (resources/tools/seedgen): C code of the Dune 2 map generator, with golden master tests.
# Its globals and function names would clash with the game, so it gets its own test executables:
# one for the default 64x64 size and one built with another size (the sources use MAP_WIDTH/MAP_HEIGHT).
set(SEEDGEN_DIR ${CMAKE_SOURCE_DIR}/resources/tools/seedgen)
set(SEEDGEN_SOURCES ${SEEDGEN_DIR}/seed.c ${SEEDGEN_DIR}/mapio.c ${SEEDGEN_DIR}/rand.c)
# the .c files use bool without stdbool.h and have narrowing initializers, so they are built as C++
set_source_files_properties(${SEEDGEN_SOURCES} PROPERTIES LANGUAGE CXX)

# campaign seeds plus a few extra
set(SEEDGEN_SEEDS_64 "{0, 1, 42, 4242, 65535, 5, 12, 16, 24, 27, 33, 53, 57, 59, 62, 89, 103, 106, 113, 117, 122, 123, 157, 173, 182, 203, 207, 223, 233, 268, 269, 298, 305, 332, 353, 355, 367, 392, 500, 501, 504, 552, 563, 582, 604, 717, 723, 1498, 2106, 2437, 2620, 2674, 3161}")
set(SEEDGEN_SEEDS_LARGE "{12, 42, 500, 3161}")

function(add_seedgen_tests target expectedDir seeds)
    add_executable(${target} ${CMAKE_CURRENT_SOURCE_DIR}/seedgen/test_seedgen.cpp ${SEEDGEN_SOURCES})
    target_include_directories(${target} PRIVATE ${SEEDGEN_DIR})
    # compat.h renames the tool's random(), which clashes with libc random() on Linux/macOS
    target_compile_options(${target} PRIVATE -w -Wno-narrowing -include ${SEEDGEN_DIR}/compat.h)
    target_compile_definitions(${target} PRIVATE
        SEEDGEN_EXPECTED_DIR="${SEEDGEN_DIR}/${expectedDir}"
        SEEDGEN_SEEDS=${seeds})
    target_link_libraries(${target} PRIVATE Catch2::Catch2WithMain)
    catch_discover_tests(${target} TEST_PREFIX "${target}.")
endfunction()

add_seedgen_tests(seedgen_tests expected "${SEEDGEN_SEEDS_64}")
add_seedgen_tests(seedgen_tests_large expected_large "${SEEDGEN_SEEDS_LARGE}")
target_compile_definitions(seedgen_tests_large PRIVATE MAP_WIDTH=128 MAP_HEIGHT=96 SEEDGEN_LARGE)
