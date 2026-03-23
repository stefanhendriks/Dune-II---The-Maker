#pragma once

#include <array>
#include <cstddef>
#include <stdexcept>

#include "include/enums.h"

// UPGRADES stuff
#define MAX_UPGRADETYPES       50    // maximum of upgrades

/**
 * Upgrades are tied to structures (ie this is used to upgrade a structure type). After
 * the upgrade is completed, the result is always an increased upgrade count.
 *
 * Then, also, this struct provides which item and what kind of item will be made available.
 */
struct s_UpgradeInfo {
    bool enabled;        // set to true to use this upgrade logic
    int icon;            // icon id
    int cost;            // price
    char description[64]; // ie: "Upgrade to 4slab"
    int buildTime;     // how long it takes to upgrade/build
    unsigned char house; // 8-bit-flag for which house this upgrade applies.
    int techLevel;      // the minimum techlevel required for this upgrade
    int atUpgradeLevel; // linear upgradeLevel per structure type, this is the nr where this upgrade is offered. (0 = start)
    int structureType;  // if > -1 then increase upgradeLevel for structureType (this is the structureType being 'upgraded')
    int needsStructureType; // the upgrade is only available when this structure is available, this is additional to the structureType property
    // above, so if you require CONSTYARD *and* RADAR you set structureType=CONSTYARD and needsStructure=RADAR, keep -1
    // if you don't require any additional structure
    eBuildType providesType;    // UNIT or STRUCTURE, etc
    int providesTypeId;   // upgrade results into getting this typeId (type depends on 'providesType') (points to s_Unit/s_Structure)
    eListType providesTypeList; // into which list will this type be made available?
    int providesTypeSubList; // and sublist
};




class cUpgradeInfos {
public:
    cUpgradeInfos()
    {}

    // accès
    s_UpgradeInfo &operator[](std::size_t i) {
        if (i >= MAX_UPGRADETYPES) throw std::out_of_range("UpgradeInfo index");
        return m_data[i];
    }

    const s_UpgradeInfo &operator[](std::size_t i) const {
        if (i >= MAX_UPGRADETYPES) throw std::out_of_range("UpgradeInfo index");
        return m_data[i];
    }

    // getter explicite
    s_UpgradeInfo &get(std::size_t i) { return (*this)[i]; }
    const s_UpgradeInfo &get(std::size_t i) const { return (*this)[i]; }

    // taille
    static constexpr std::size_t size() noexcept { return MAX_UPGRADETYPES; }

    // recherche/existence
    bool valid(std::size_t i) const noexcept { return i < MAX_UPGRADETYPES; }


    // itérateurs
    auto begin() noexcept { return m_data.begin(); }
    auto end() noexcept { return m_data.end(); }
    auto begin() const noexcept { return m_data.begin(); }
    auto end() const noexcept { return m_data.end(); }

private:
    std::array<s_UpgradeInfo, MAX_UPGRADETYPES> m_data;
};
