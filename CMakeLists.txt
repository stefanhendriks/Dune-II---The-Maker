cmake_minimum_required(VERSION 3.21)

set(CMAKE_CXX_STANDARD 20)

project(d2tm)

file(GLOB d2tm_SRC
        "*.h"
        "*.cpp"
        "building/*.h"
        "building/*.cpp"
        "controls/*.h"
        "controls/*.cpp"
        "controls/mousestates/*.h"
        "controls/mousestates/*.cpp"
        "data/*.h"
        "data/*.cpp"
        "drawers/*.h"
        "drawers/*.cpp"
        "gameobjects/*.h"
        "gameobjects/*.cpp"
        "gameobjects/structures/*.h"
        "gameobjects/structures/*.cpp"
        "gameobjects/units/*.h"
        "gameobjects/units/*.cpp"
        "gameobjects/projectiles/*.h"
        "gameobjects/projectiles/*.cpp"
        "gameobjects/particles/*.h"
        "gameobjects/particles/*.cpp"
        "observers/*.h"
        "observers/*.cpp"
        "gamestates/*.h"
        "gamestates/*.cpp"
        "gui/*.h"
        "gui/*.cpp"
        "gui/actions/*.h"
        "gui/actions/*.cpp"
        "include/*.h"
        "mentat/*.h"
        "mentat/*.cpp"
        "managers/*.h"
        "managers/*.cpp"
        "map/*.h"
        "map/*.cpp"
        "player/*.h"
        "player/*.cpp"
        "player/brains/*.h"
        "player/brains/*.cpp"
        "player/brains/missions/*.h"
        "player/brains/missions/*.cpp"
        "player/brains/missions/campaign/*.h"
        "player/brains/missions/campaign/*.cpp"
        "player/brains/missions/skirmish/*.h"
        "player/brains/missions/skirmish/*.cpp"
        "player/brains/superweapon/*.h"
        "player/brains/superweapon/*.cpp"
        "player/*.h"
        "player/*.cpp"
        "sidebar/*.h"
        "sidebar/*.cpp"
        "upgrade/*.h"
        "upgrade/*.cpp"
        "utils/*.h"
        "utils/*.cpp"
        )

add_executable(${PROJECT_NAME}
  ${d2tm_SRC}
  Script.rc
)

target_include_directories(${PROJECT_NAME}
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(${PROJECT_NAME}
  PRIVATE
    alleg
    almp3dll
    alfontdll
    fmt::fmt
    -static-libgcc
    -static-libstdc++
)

target_compile_options(${PROJECT_NAME}
  PRIVATE
    "-Werror"
    "-Wall"
    "-Wextra"
    "-Wpedantic"
    "-Wnon-virtual-dtor"
    "-Wuseless-cast"
    "-Wredundant-tags"
    "-Wzero-as-null-pointer-constant"
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_RUNTIME_DLLS:${PROJECT_NAME}> $<TARGET_FILE_DIR:${PROJECT_NAME}>
  COMMAND_EXPAND_LISTS
)

add_subdirectory(dependencies)
