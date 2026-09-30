#pragma once

#include "include/cAssert.h"

// what type of thing do we build?
// a unit/structure of something special (super weapon)

// BE MINDFUL when changing order of this enum, as it is used. If somewhere the 0/1 (ordinal) is stored and assumes
// it reflects the values below, it might break when changing the order.
enum eBuildType {
    STRUCTURE, // 0
    UNIT,      // 1
    UPGRADE,   // 2
    SPECIAL,   // 3
    BULLET,    // 4 (ie, used for super weapon)
    UNKNOWN    // 5 -> use for unknown
};

inline const char *eBuildTypeString(const eBuildType &buildType)
{
    switch (buildType) {
        case eBuildType::SPECIAL:
            return "SPECIAL";
        case eBuildType::UNIT:
            return "UNIT";
        case eBuildType::STRUCTURE:
            return "STRUCTURE";
        case eBuildType::BULLET:
            return "BULLET";
        case eBuildType::UPGRADE:
            return "UPGRADE";
        case eBuildType::UNKNOWN:
            return "UNKNOWN";
        default:
            d2tm_assert(false && "Undefined buildType?");
            break;
    }
    return "";
}

namespace buildOrder {
enum eBuildOrderState {
    PROCESSME,
    BUILDING,
    REMOVEME
};

inline const char *eBuildOrderStateString(const eBuildOrderState &state)
{
    switch (state) {
        case eBuildOrderState::PROCESSME:
            return "PROCESSME";
        case eBuildOrderState::REMOVEME:
            return "REMOVEME";
        case eBuildOrderState::BUILDING:
            return "BUILDING";
        default:
            d2tm_assert(false && "Unknown eBuildOrderState");
            break;
    }
    return "";
}
}

enum eDeployTargetType {
    TARGET_NONE,

    /**
     * Player specifies exactly cell where to deploy.
     */
    TARGET_SPECIFIC_CELL,

    /**
     * Player specifies cell where to deploy, but the actual cell to deploy
     * is determined by make it more inaccurate. The inaccuracy is determined by
     * a different variable.
     */
    TARGET_INACCURATE_CELL,
};

/**
 * Used to determine how deployment is arranged. Usually AT_STRUCTURE is default behavior (ie for spawning next
 * of a structure. AT_RANDOM_CELL is used for Fremen Super Weapon.
 */
enum eDeployFromType {
    /**
     * Random cell
     */
    AT_RANDOM_CELL,

    /**
     * Deploy at structure
     */
    AT_STRUCTURE
};
