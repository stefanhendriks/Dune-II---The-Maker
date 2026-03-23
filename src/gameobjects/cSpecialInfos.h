#include "include/enums.h"



// SPECIALS (super weapons) stuff
#define MAX_SPECIALTYPES       10    // maximum of specialInfo



/**
 * Upgrades are tied to structures (ie this is used to upgrade a structure type). After
 * the upgrade is completed, the result is always an increased upgrade count.
 *
 * Then, also, this struct provides which item and what kind of item will be made available.
 */
struct s_SpecialInfo {
    int icon;            // icon id

    char description[64]; // ie: "Upgrade to 4slab"

    int buildTime;     // how long it takes to 'build' (ie wait before ready)

    eBuildType providesType;
    int providesTypeId;   // slot of type it points to (typeId)

    int units;   // amount of units to spawn at once

    eDeployFromType deployFrom; // for Fremen, deploys unit to random cell

    eDeployTargetType deployTargetType; // for ie DeathHand, how 'accurate' the deploy target will become

    /**
     * how accurate (for deployTargetType == INACCURATE), ie the amount of cells it can be off.
     * A 0 means 100% accurate (ie nothing to fudge), 1 means 1 cell off, etc.
     */
    int deployTargetPrecision;

    unsigned char house; // which house(s) have access to this?

    bool autoBuild; // if true, then this item is built automatically

    int deployAtStructure; // if deployFrom == STRUCTURE, then this is the structure type to deploy at

    eListType listType;
    int subListId;
};
