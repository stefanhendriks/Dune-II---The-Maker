/**
 * @file FogMemory.hpp
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

#include "include/enums/MapEnums.h"
#include <optional>
#include <vector>

/**
 * What the player remembers of the terrain of a cell: the values as they were
 * at the moment the cell dropped out of sight.
 */
struct s_RememberedTerrain {
    int type = 0;
    int tile = 0;
    int smudgetile = -1;
    std::optional<SmudgeType> smudgetype;
};

/**
 * What the player remembers of a structure on one cell: the slice of the structure that
 * covers the cell, as it looked when the cell dropped out of sight.
 */
struct s_RememberedStructure {
    int type = 0;
    int ownerId = 0;
    int frame = 0;
    int offsetX = 0; // cell offset within the structure (0,0 = top left cell)
    int offsetY = 0;
};

/**
 * What the player remembers of a unit on one cell: the base unit type only, drawn as
 * hull plus turret. No unit ID (unit slots get reused), no house color, no position within
 * the cell: a unit that moved on stays remembered at the cell it was last seen on.
 */
struct s_RememberedUnit {
    int type = 0;
    int bodyIndex = 0; // draw index (facing) of the hull
    int headIndex = 0; // draw index (facing) of the turret, ignored for units without one
    int frame = 0;
};

/**
 * Per-cell memory of what was last seen, for one player. Pure data and rules
 * (no SDL, no game objects), so it can be unit tested.
 *
 * Rules: a cell that goes from seen to unseen is remembered; a cell that is
 * seen forgets its memory (live data is drawn again).
 */
class FogMemory {
public:
    FogMemory() = default;

    explicit FogMemory(int maxCells) : m_cells(maxCells), m_structures(maxCells), m_units(maxCells) {}

    /**
     * Apply the rule for one cell after a fog of war update.
     * @param live current terrain of the cell, only copied when the cell just dropped out of sight
     */
    void update(int cell, bool wasSeen, bool isSeen, const s_RememberedTerrain &live)
    {
        if (cell < 0 || cell >= static_cast<int>(m_cells.size())) return;

        if (isSeen) {
            m_cells[cell].reset();
        } else if (wasSeen) {
            m_cells[cell] = live;
        }
    }

    /**
     * Same rule as update(), for the structure on the cell (nullopt when there is none).
     * A remembered structure is purely visual: it is not an object and can never be targeted.
     */
    void updateStructure(int cell, bool wasSeen, bool isSeen, const std::optional<s_RememberedStructure> &live)
    {
        if (cell < 0 || cell >= static_cast<int>(m_structures.size())) return;

        if (isSeen) {
            m_structures[cell].reset();
        } else if (wasSeen) {
            m_structures[cell] = live;
        }
    }

    /**
     * Same rule as update(), for the unit on the cell (nullopt when there is none).
     * A remembered unit is purely visual: it is not an object and can never be targeted.
     */
    void updateUnit(int cell, bool wasSeen, bool isSeen, const std::optional<s_RememberedUnit> &live)
    {
        if (cell < 0 || cell >= static_cast<int>(m_units.size())) return;

        if (isSeen) {
            m_units[cell].reset();
        } else if (wasSeen) {
            m_units[cell] = live;
        }
    }

    /**
     * @return the remembered unit of this cell, or nullptr when none is remembered
     */
    [[nodiscard]] const s_RememberedUnit *getUnit(int cell) const
    {
        if (cell < 0 || cell >= static_cast<int>(m_units.size())) return nullptr;
        return m_units[cell].has_value() ? &*m_units[cell] : nullptr;
    }

    /**
     * @return the remembered structure slice of this cell, or nullptr when none is remembered
     */
    [[nodiscard]] const s_RememberedStructure *getStructure(int cell) const
    {
        if (cell < 0 || cell >= static_cast<int>(m_structures.size())) return nullptr;
        return m_structures[cell].has_value() ? &*m_structures[cell] : nullptr;
    }

    /**
     * @return what was remembered for this cell, or nullptr when nothing is (draw the live terrain)
     */
    [[nodiscard]] const s_RememberedTerrain *getTerrain(int cell) const
    {
        if (cell < 0 || cell >= static_cast<int>(m_cells.size())) return nullptr;
        return m_cells[cell].has_value() ? &*m_cells[cell] : nullptr;
    }

private:
    std::vector<std::optional<s_RememberedTerrain>> m_cells;
    std::vector<std::optional<s_RememberedStructure>> m_structures;
    std::vector<std::optional<s_RememberedUnit>> m_units;
};
