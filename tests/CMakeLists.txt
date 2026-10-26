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
endif()

include(CTest)
include(Catch)
catch_discover_tests(d2tm_tests)
