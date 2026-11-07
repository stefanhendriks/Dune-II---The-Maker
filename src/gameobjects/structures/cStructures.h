/**
 * @file cStructures.h
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
#include "include/cAssert.h"
#include <cstddef>
#include <memory>
#include <vector>
#include "definitions.h"
#include "gameobjects/structures/cAbstractStructure.h"

/**
 * Container wrapper for the global g_pStructure array
 * Provides safe indexed access and standard container interface
 */
class cStructures {
public:
    cStructures() = default;

    // Read access: the pool owns the structures, callers only borrow them.
    cAbstractStructure *operator[](std::size_t index) const {
        d2tm_assert(index < MAX_STRUCTURES && "Index out of bounds for cStructure");
        return m_values[index].get();
    }

    // Puts a structure in a slot, the pool takes ownership. The slot index is the structure id.
    void place(std::size_t index, std::unique_ptr<cAbstractStructure> structure) {
        d2tm_assert(index < MAX_STRUCTURES && "Index out of bounds for cStructure");
        m_values[index] = std::move(structure);
    }

    // Empties a slot and destroys the structure in it.
    void remove(std::size_t index) {
        d2tm_assert(index < MAX_STRUCTURES && "Index out of bounds for cStructure");
        m_values[index].reset();
    }

    // Empties a slot, but keeps the structure alive until destroyDetached() is called.
    // Used by a structure that dies: it frees its slot while it is still running its die() code.
    void detach(std::size_t index) {
        d2tm_assert(index < MAX_STRUCTURES && "Index out of bounds for cStructure");
        if (m_values[index]) {
            m_detached.push_back(std::move(m_values[index]));
        }
    }

    void destroyDetached() { m_detached.clear(); }

    std::size_t size() const noexcept { return m_values.size(); }

    auto begin() noexcept { return m_values.begin(); }
    auto end() noexcept { return m_values.end(); }
    auto begin() const noexcept { return m_values.begin(); }
    auto end() const noexcept { return m_values.end(); }

private:
    static constexpr std::size_t MAX_STRUCTURES_COUNT = MAX_STRUCTURES;
    std::array<std::unique_ptr<cAbstractStructure>, MAX_STRUCTURES_COUNT> m_values{};
    std::vector<std::unique_ptr<cAbstractStructure>> m_detached;
};