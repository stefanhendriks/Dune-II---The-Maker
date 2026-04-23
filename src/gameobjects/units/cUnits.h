/*

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefan@fundynamic.com
  Website: http://dune2themaker.fundynamic.com

  2001 - 2022 (c) code by Stefan Hendriks

  */

#pragma once

#include <array>
#include "definitions.h"
#include "cUnit.h"

struct sGameServices;

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
     * @param index The unit index
     * @return Reference to cUnit object
     */
    cUnit& operator[](int index);
    const cUnit& operator[](int index) const;

    /**
     * Safe access with bounds checking
     * @param index The unit index
     * @return Pointer to cUnit if valid, nullptr otherwise
     */
    cUnit* getUnit(int index);
    const cUnit* getUnit(int index) const;

    /**
     * Get the capacity (maximum number of units)
     * @return The capacity
     */
    static constexpr int capacity() { return MAX_UNITS_CAPACITY; }

    /**
     * Get the number of units available (returns capacity)
     * @return The size
     */
    static constexpr int size() { return MAX_UNITS_CAPACITY; }

    /**
     * Find the first available (invalid) unit slot
     * @return Index of available unit, or -1 if none available
     */
    int findAvailableSlot() const;

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
     * @return
     */
    static int unitCreate(int iCll, int unitType, int iPlayer, bool bOnStart, bool isReinforcement = false, float hpPercentage =1.0f);


private:
    // Maximum number of units
    static constexpr int MAX_UNITS_CAPACITY = 300;
    std::array<cUnit, MAX_UNITS_CAPACITY> m_units;
};
