cmake_minimum_required(VERSION 3.10)

# set the project name
project(d2tm)

link_directories("lib/mingw")

# add the executableS
add_executable(d2tm Script.rc)
target_link_libraries(d2tm fblend alleg aldat almp3dll alfontdll -static-libgcc -static-libstdc++)

file(GLOB d2tm_SRC
        "*.h"
        "*.cpp"
        "building/*.h"
        "building/*.cpp"
        "controls/*.h"
        "controls/*.cpp"
        "data/*.h"
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

# specify sources
target_sources(d2tm
        PUBLIC ${d2tm_SRC})

target_include_directories(d2tm
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/building ${CMAKE_CURRENT_SOURCE_DIR}/include)

