#pragma once

#include <array>
#include <cstddef>
#include <stdexcept>

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



class cSpecialInfos {
public:
    cSpecialInfos()
    {}

    // accès
    s_SpecialInfo &operator[](std::size_t i) {
        if (i >= MAX_SPECIALTYPES) throw std::out_of_range("SpecialInfo index");
        return m_data[i];
    }

    const s_SpecialInfo &operator[](std::size_t i) const {
        if (i >= MAX_SPECIALTYPES) throw std::out_of_range("SpecialInfo index");
        return m_data[i];
    }

    // getter explicite
    s_SpecialInfo &get(std::size_t i) { return (*this)[i]; }
    const s_SpecialInfo &get(std::size_t i) const { return (*this)[i]; }

    // taille
    static constexpr std::size_t size() noexcept { return MAX_SPECIALTYPES; }

    // recherche/existence
    bool valid(std::size_t i) const noexcept { return i < MAX_SPECIALTYPES; }


    // itérateurs
    auto begin() noexcept { return m_data.begin(); }
    auto end() noexcept { return m_data.end(); }
    auto begin() const noexcept { return m_data.begin(); }
    auto end() const noexcept { return m_data.end(); }

private:
    std::array<s_SpecialInfo, MAX_SPECIALTYPES> m_data;
};
