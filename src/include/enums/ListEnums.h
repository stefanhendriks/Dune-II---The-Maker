#pragma once

enum class eListType {
    LIST_NONE,  // no list (empty list)
    LIST_CONSTYARD,
    LIST_FOOT_UNITS, // infantry, soldier, trooper, troopers, etc.
    LIST_UNITS, // quad, trike, tanks, air units
    LIST_STARPORT, // special list to order stuff
    LIST_PALACE, // REMOVE? (ie super weapon icon should always show top left/right?)
    LIST_UPGRADES, // possible sUpgradeInfo
};

static const eListType AllListTypes[] = {
    eListType::LIST_NONE,
    eListType::LIST_CONSTYARD,
    eListType::LIST_FOOT_UNITS,
    eListType::LIST_UNITS,
    eListType::LIST_STARPORT,
    eListType::LIST_PALACE,
    eListType::LIST_UPGRADES
};

inline eListType eListTypeFromInt(int value)
{
    switch (value) {
        case 0:
            return eListType::LIST_NONE;
        case 1:
            return eListType::LIST_CONSTYARD;
        case 2:
            return eListType::LIST_FOOT_UNITS;
        case 3:
            return eListType::LIST_UNITS;
        case 4:
            return eListType::LIST_STARPORT;
        case 5:
            return eListType::LIST_PALACE;
        case 6:
            return eListType::LIST_UPGRADES;
    }
    return eListType::LIST_NONE;
}

inline int eListTypeAsInt(eListType value)
{
    switch (value) {
        case eListType::LIST_NONE:
            return 0;
        case eListType::LIST_CONSTYARD:
            return 1;
        case eListType::LIST_FOOT_UNITS:
            return 2;
        case eListType::LIST_UNITS:
            return 3;
        case eListType::LIST_STARPORT:
            return 4;
        case eListType::LIST_PALACE:
            return 5;
        case eListType::LIST_UPGRADES:
            return 6;
    }
    return 0;
}

// the BuildingListItemState
enum eBuildingListItemState {
    AVAILABLE,
    BUILDING,
    PAUSED,
    PENDING_UPGRADE,    // an upgrade is blocking this item to be built
    PENDING_BUILDING,   // another item being built is blocking this item to be upgraded
    UNAVAILABLE
};
