#pragma once

#include <array>
#include <cstddef>
#include <stdexcept>

#include "include/enums.h"

#include <SDL2/SDL.h>
class Texture;

// UNITS stuff
#define MAX_UNITTYPES       50    // maximum of unit types

// Unit properties
// the s_UnitInfo struct is holding all properties for a specific unit type.
// note: The unit properties are actually set in common.cpp
// note: Some values are now read via INI.CPP (GAME.INI reader).


struct s_UnitInfo {
    SDL_Surface  *bmp;         // each unit has a reference to his 8 bit bitmap.
    SDL_Surface  *top;         // top of unit
    Texture *shadow;      // shadow of unit (24 bit)

    int    bmp_width;     // width of each unit frame (in pixels).
    int    bmp_height;    // Height of the unit, for each animating frame.
    int    bmp_frames;
    int    bmp_startpixel; // The X axis starting pixel (i.e. for the harvester)

    int    hp;             // Amount of hitpoints at maximum
    int    speed;          // speed (0 being super fast, the higher the number, the slower).
    int    turnspeed;
    int    attack_frequency;  // timer for attacking
    int    next_attack_frequency;  // in case a second shot is fired, this is the delay used. (-1 by default, if
    // not specified, this will be 1/4th of attack_frequency)

    int    buildTime;        // how long it takes for building this thing
    int    bulletType;        // type of bullet to fire normally
    int    bulletTypeSecondary;   // type of bullet when enemy is within a close range
    int    fireSecondaryWithinRange; // if specified, the secondary bullet type is fired when enemy is within this range
    int    max_bullets;       // Maximum of 'bullets' it can fire at a time
    int    sight;             // Sight of a unit (in radius)
    int    range;             // Range of a unit (in radius)
    int    icon;              // icon id (taken from gfxInter data file)
    int    cost;              // cost/price

    bool airborn;             // does this unit fly?
    bool infantry;            // legged unit? (infantry)
    bool free_roam;           // (airborn only) - does it free roam?
    bool fireTwice;           // can shoot 2 times?
    float fireTwiceHpThresholdFactor; // this threshold has to be met in order to have unit fire twice. Usually same as smokeHpFactor
    bool squish;              // can this unit squish infantry?
    bool canBeSquished;       // can this unit be squished (is this a squishable unit)
    int dieWhenLowerThanHP;   // A unit that 'dies'(disappears) when health < this value. By default this is 1;
    int appetite;     // Amount of units this can eat, before 'dying'

    char name[64];            // name of unit
    int structureTypeItLeavesFrom; // the type of structure this unit will 'leave' from when it is built. (ie Quad from Light Factory, etc)
    bool queuable;        // can this item be queued in the buildList? (default = true for units)

    // UNIT : HARVESTER specific types
    int credit_capacity;      // max credits capacity
    int harvesting_speed;     // speed rate for harvesting (time)
    int harvesting_amount;    // value rate for harvesting (amount)

    // house specific rates

    // list related
    eListType listType;               // ie LIST_FOOT_UNITS or LIST_UNITS
    int subListId;            // ie SUBLIST_LIGHTFCTRY

    // attack related
    bool canAttackAirUnits;   // ie for rocket typed units
    bool canAttackUnits;   // a unit used for attacking other units? (ie, mvc or harvester is no)
    bool canEnterAndDamageStructure;  // can this unit enter a structure and damage it? (and eventually capture?)
    bool attackIsEnterStructure;      // for saboteur only really
    float damageOnEnterStructure;     // the damage inflicted to a structure when entered

    bool renderSmokeOnUnitWhenThresholdMet; // if yes, spawn smoke particle (and attach to unit) when hp factor met
    float smokeHpFactor; // 0.5 = 50% health, etc. When health is getting lower than this and unit is damaged, spawn smoke particle,
    bool canGuard; // if true, the unit has guarding behavior (will try to attack nearby enemy units)
};





class cUnitInfos {
public:
    cUnitInfos()
    {}

    // accès
    s_UnitInfo &operator[](std::size_t i) {
        if (i >= MAX_UNITTYPES) throw std::out_of_range("UnitInfo index");
        return m_data[i];
    }

    const s_UnitInfo &operator[](std::size_t i) const {
        if (i >= MAX_UNITTYPES) throw std::out_of_range("UnitInfo index");
        return m_data[i];
    }

    // getter explicite
    s_UnitInfo &get(std::size_t i) { return (*this)[i]; }
    const s_UnitInfo &get(std::size_t i) const { return (*this)[i]; }

    // taille
    static constexpr std::size_t size() noexcept { return MAX_UNITTYPES; }

    // recherche/existence
    bool valid(std::size_t i) const noexcept { return i < MAX_UNITTYPES; }


    // itérateurs
    auto begin() noexcept { return m_data.begin(); }
    auto end() noexcept { return m_data.end(); }
    auto begin() const noexcept { return m_data.begin(); }
    auto end() const noexcept { return m_data.end(); }

private:
    std::array<s_UnitInfo, MAX_UNITTYPES> m_data;
};
