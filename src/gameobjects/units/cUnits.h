/**
 * @file cUnits.h
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

#pragma once

#include <array>
#include "definitions.h"
#include "cUnit.h"

struct sGameServices;
class cInfoContext;
class cGameObjectContext;
class cEventEmitter;
class cMap;
class cLog;

/**
 * Manager class that encapsulates the global g_Unit array
 * Provides safe access to units with bounds checking and convenient methods
 */
class cUnits {
public:
    /**
     * Default constructor
     */
    cUnits();
    void serviceInit(sGameServices* services);

    /**
     * Non-copyable
     */
    cUnits(const cUnits&) = delete;
    cUnits& operator=(const cUnits&) = delete;

    /**
     * Access unit by index (compatible with array syntax)
     */
    cUnit* operator[](int index);
    const cUnit* operator[](int index) const;

    /**
     * Safe access with bounds checking
     * @param index The unit index
     * @return Pointer to cUnit if valid, nullptr otherwise
     */
    cUnit* getUnit(int index);
    const cUnit* getUnit(int index) const;

    /**
     * Shorthand for the "index > -1 and unit is valid" check
     * @param index The unit index, -1 means no unit
     * @return true when index refers to a valid unit
     */
    bool isValidUnit(int index) const;

    /**
     * Maximum number of units that can be in the game (the unit limit)
     */
    static constexpr int MAX_UNITS_CAPACITY = 300;

    /**
     * Extra slots on top of the unit limit, used only by carry-alls that are spawned to deliver a unit. This way
     * a delivery never competes with regular units for a slot.
     */
    static constexpr int CARRYALL_RESERVE = 10;

    /**
     * Size of the unit array: the unit limit plus the carry-all reserve
     */
    static constexpr int TOTAL_SLOTS = MAX_UNITS_CAPACITY + CARRYALL_RESERVE;

    /**
     * Get the capacity (size of the unit array, including the carry-all reserve)
     * @return The capacity
     */
    static constexpr int capacity() { return TOTAL_SLOTS; }

    /**
     * Get the number of units available (returns capacity)
     * @return The size
     */
    static constexpr int size() { return TOTAL_SLOTS; }

    /**
     * Find the first available (invalid) unit slot (the carry-all reserve is not considered)
     * @return Index of available unit, or -1 if none available
     */
    int findAvailableSlot() const;

    /**
     * All unit slots are in use, so no new unit can be created
     * @return true when no slot is free
     */
    bool isUnitLimitReached() const { return findAvailableSlot() < 0; }

    /**
     * Count valid units
     * @return Number of valid units
     */
    int getValidUnitsCount() const;

    /**
     * Begin iterator for range-based for loops
     */
    auto begin() { return m_units.begin(); }
    auto begin() const { return m_units.begin(); }

    /**
     * End iterator for range-based for loops
     */
    auto end() { return m_units.end(); }
    auto end() const { return m_units.end(); }

    void move_to(int icell);

    /**
     * Creates a new unit, when bOnStart is true, it will prevent AI players from moving a unit immediately a bit.
     * Assumes the creation of a unit is NOT a reinforcement.
     *
     * @param iCll
     * @param unitType
     * @param iPlayer
     * @param bOnStart (if true, then AI will *not* move unit away immediately to a nearby random position)
     * @return
     */
    // static int unitCreate(int iCll, int unitType, int iPlayer, bool bOnStart);
    /**
    * creates a unit, the isReinforcement flag is true when the unit is created for / by reinforcements. This
    * flag will make sure to trigger a different event type (not CREATED, but REINFORCED) so that we can distinguish
    * between them.
    *
    * @param iCll
    * @param unitType
    * @param iPlayer
    * @param bOnStart
    * @param isReinforcement
    * @return
    */
    // static int unitCreate(int iCll, int unitType, int iPlayer, bool bOnStart, bool isReinforcement);
    /**
     * Creates a new unit, when bOnStart is true, it will prevent AI players from moving a unit immediately a bit.
     *
     *
     * @param iCll
     * @param unitType
     * @param iPlayer
     * @param bOnStart
     * @param hpPercentage multiplies with hp of unit type for starting hp. 1.0 means 100% health
     * @param isReinforement flag to set on event
     * @param useCarryAllReserve when true, the unit is placed in the carry-all reserve slots first (for spawned carry-alls)
     * @return
     */
    static int unitCreate(cGameObjectContext* objects, cInfoContext* infos, cEventEmitter* eventEmitter, int iCll, int unitType, int iPlayer, bool bOnStart, bool isReinforcement = false, float hpPercentage = 1.0f, bool useCarryAllReserve = false);


private:
    cInfoContext *m_infos = nullptr;
    cGameObjectContext *m_objects = nullptr;
    cMap* m_map = nullptr;
    std::array<cUnit, TOTAL_SLOTS> m_units;
};
