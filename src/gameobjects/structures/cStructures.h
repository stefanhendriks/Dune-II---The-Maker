/**
 * @file cStructures.h
 *
 * Dune 2 - The Maker
 *
 * @author Stefan Hendriks & the D2TM Team
 * @www http://www.dune2themaker.com
 * @copyright Copyright (c) 2001 - 2026 D2TM Team
 * @license This software is released under the MIT License.
 *
 * Note: Dune 2 is a trademark of Westwood Studios/Electronic Arts.
 *
 * This is a non-commercial educational project.
 */

#pragma once

#include <array>
#include "include/cAssert.h"
#include <cstddef>
#include "definitions.h"
#include "gameobjects/structures/cAbstractStructure.h"

/**
 * Container wrapper for the global g_pStructure array
 * Provides safe indexed access and standard container interface
 */
class cStructures {
public:
    cStructures() = default;

    ~cStructures() {
        for (auto* s : m_values) {
            delete s;
        }
    }

    cAbstractStructure* &operator[](std::size_t index) {
        d2tm_assert(index < MAX_STRUCTURES && "Index out of bounds for cStructure");
        return m_values[index];
    }

    cAbstractStructure* const &operator[](std::size_t index) const {
        d2tm_assert(index < MAX_STRUCTURES && "Index out of bounds for cStructure");
        return m_values[index];
    }

    cAbstractStructure** data() noexcept { return m_values.data(); }
    const cAbstractStructure* const* data() const noexcept { return m_values.data(); }

    std::size_t size() const noexcept { return m_values.size(); }

    auto begin() noexcept { return m_values.begin(); }
    auto end() noexcept { return m_values.end(); }
    auto begin() const noexcept { return m_values.begin(); }
    auto end() const noexcept { return m_values.end(); }

private:
    static constexpr std::size_t MAX_STRUCTURES_COUNT = MAX_STRUCTURES;
    std::array<cAbstractStructure*, MAX_STRUCTURES_COUNT> m_values{};
};