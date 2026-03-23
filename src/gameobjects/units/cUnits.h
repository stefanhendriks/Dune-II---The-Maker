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

private:
    // Maximum number of units
    static constexpr int MAX_UNITS_CAPACITY = 300;
    std::array<cUnit, MAX_UNITS_CAPACITY> m_units;
};
