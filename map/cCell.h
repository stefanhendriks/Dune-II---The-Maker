#ifndef CCELL_H_
#define CCELL_H_

struct tCell {
    int credits;        // harvesting
    int type;           // refers to gfxdata
    int tile;           // the tile to draw (* 32 (on x scale))
    int smudgetile;     // smudge tile id (drawn upon the tile)
    int smudgetype;     // smudge type id
    int health;         // hitpoints for specific tile (used for walls)
    bool passable;      // passable? > pathfinder (TRUE = default)
    bool passableFoot;  // passable for foot units? > pathfinder (TRUE = default)

    bool iVisible[MAX_PLAYERS];      // visibility for <player>

    // there are 4 'dimensions' of this cell. Each represents the perspective of
    // an entity that is either a unit, structure, the 'worms' dimension and the 'air' dimension.
    int id[4];          // ID of
                        // 0 = unit
                        // 1 = structure
                        // 2 = air
                        // 3 = worm
};

#endif /* CCELL_H_ */
