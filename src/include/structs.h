#pragma once

#include "enums.h"

#include <vector>
#include <string>

#include <SDL2/SDL.h>

class Texture;

struct s_FlagInfo {
    bool big = true; // big flag or not?
    int relX = 0;
    int relY = 0; // relative X, Y position compared to top-left of structure
};

// Structure types
struct s_StructureInfo {
    // Structures
    SDL_Surface *bmp;         // a pointer to the 8 bit bitmap
    Texture *shadow;	   // a pointer to the shadow bitmap
    SDL_Surface *flash;	   // a pointer to the flash bitmap (used to overlay when unit exits building for light/heavy fctry)

    std::vector<s_FlagInfo> flags;

    int bmp_width;       // width structure (in pixels)
    int bmp_height;      // height structure (in pixels)

    int list;            // list this structure has!

    int hp;              // max hp
    int fixhp;	   	   // how much you get for fixing (reparing) this thing

    int bullets;         // type of bullet

    int buildTime;      // how long it takes for building this structure

    int power_drain;       // the power that this building drains...
    int power_give;        // the power that this building gives...

    int sight;

    int fadecol;         // Fading color (if needed), -1 is none, > -1 means starting color
    int fademax;         // Max fading color (assuming fadecol < fademax)

    int icon;            // icon id

    int cost;            // price

    char name[64];       // name

    bool queuable;        // can this item be queued in the buildList? (default = false)

    bool configured;     // is this structure configured? (poor man solution)

    // Slab related
    bool hasConcrete;   // if true, then structure gets pavement (for AI) or needs pavement (impacts health), and can decay

    // Turret specific:
    bool canAttackAirUnits;
    bool canAttackGroundUnits;
    int fireRate;         // delay before firing

};


struct s_TerrainInfo {
    int bloomTimerDuration;         // duration from bloom timer
    int terrainSpiceMinSpice;       // minimal spice in TERRAIN_SPICE cells
    int terrainSpiceMaxSpice;       // maximal spice in TERRAIN_SPICE cells
    int terrainSpiceHillMinSpice;   // minimal spice in TERRAIN_SPICEHILL cells
    int terrainSpiceHillMaxSpice;   // maximal spice in TERRAIN_SPICEHILL cells
    int terrainWallHp;              // HP of a terrain WALL
};