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
 * Per-cell memory of what was last seen, for one player. Pure data and rules
 * (no SDL, no game objects), so it can be unit tested.
 *
 * Rules: a cell that goes from seen to unseen is remembered; a cell that is
 * seen forgets its memory (live data is drawn again).
 */
class FogMemory {
public:
    FogMemory() = default;

    explicit FogMemory(int maxCells) : m_cells(maxCells) {}

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
     * @return what was remembered for this cell, or nullptr when nothing is (draw the live terrain)
     */
    [[nodiscard]] const s_RememberedTerrain *getTerrain(int cell) const
    {
        if (cell < 0 || cell >= static_cast<int>(m_cells.size())) return nullptr;
        return m_cells[cell].has_value() ? &*m_cells[cell] : nullptr;
    }

private:
    std::vector<std::optional<s_RememberedTerrain>> m_cells;
};
