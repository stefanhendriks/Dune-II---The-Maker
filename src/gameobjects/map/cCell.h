/**
 * @file cCell.h
 *
 * Dune 2 - The Maker
 *
 * @author Stefan Hendriks & the D2TM Team
 * @www http://www.dune2themaker.com
 * @copyright Copyright (c) 2002 - 2026 Stefan Hendriks and contributors
 * @license This software is released under the MIT License. See LICENSE.md.
 *
 * Note: Dune 2 is a trademark of Westwood Studios/Electronic Arts.
 *
 * This is a non-commercial educational project.
 */

#ifndef CCELL_H_
#define CCELL_H_

#include "definitions.h"
#include "enums.h"
#include <optional>

struct tCell {
    int credits;        // amount of credits that can be harvested from this tile
    int type;           // refers to gfxdata (terrain type)
    int tile;           // the tile to draw
    int smudgetile;     // smudge tile id (drawn upon the tile)
    std::optional<SmudgeType> smudgetype;     // smudge type id
    int health;         // hitpoints for specific tile (used for walls)
    bool passable;      // passable? > pathfinder (TRUE = default) (passable for ground units, non infantry)
    bool passableFoot;  // passable for foot units? > pathfinder (TRUE = default)

    bool iVisible[MAX_PLAYERS];      // discovered by <player> (shroud lifted, permanent)
    bool iSeen[MAX_PLAYERS];         // within sight range of <player> right now (fog of war)

    // there are 4 'dimensions' of this cell. Each represents the perspective of
    // an entity that is either a unit, structure, the 'worms' dimension and the 'air' dimension.
    int id[4];          // ID of
    // 0 = unit
    // 1 = structure
    // 2 = air
    // 3 = worm
};

#endif /* CCELL_H_ */
