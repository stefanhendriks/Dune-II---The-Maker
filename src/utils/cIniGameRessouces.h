#pragma once

#include "include/enums.h"

#include <memory>

struct s_TerrainInfo;

void install_structures();
void install_units();
void install_bullets();
void install_upgrades();
void install_specials();
void install_particles();

void install_terrain(std::shared_ptr<s_TerrainInfo>& terrainInfo);