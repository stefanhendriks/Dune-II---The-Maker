#pragma once

#include "include/enums.h"

#include <memory>

struct s_TerrainInfo;

class IniGameRessources {
public:
    static void install_structures();
    static void install_units();
    static void install_bullets();
    static void install_upgrades();
    static void install_specials();
    static void install_particles();
    static void install_terrain(std::shared_ptr<s_TerrainInfo>& terrainInfo);
};

