/**
 * @file cBullets.h
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
#include "gameobjects/projectiles/bullet.h"

struct sGameServices;

/**
 * Container wrapper for the global g_Bullet array
 * Provides safe indexed access and standard container interface
 */
class cBullets {
public:
    cBullets() = default;
    void serviceInit(sGameServices *services);

    cBullet &operator[](std::size_t index) {
        d2tm_assert(index < MAX_BULLETS && "Index out of bounds for g_Bullet");
        return m_values[index];
    }

    const cBullet &operator[](std::size_t index) const {
        d2tm_assert(index < MAX_BULLETS && "Index out of bounds for g_Bullet");
        return m_values[index];
    }

    cBullet *data() noexcept { return m_values.data(); }
    const cBullet *data() const noexcept { return m_values.data(); }

    std::size_t size() const noexcept { return m_values.size(); }

    auto begin() noexcept { return m_values.begin(); }
    auto end() noexcept { return m_values.end(); }
    auto begin() const noexcept { return m_values.begin(); }
    auto end() const noexcept { return m_values.end(); }

private:
    static constexpr std::size_t MAX_BULLETS = 300;     // max of bullets in the game at once
    std::array<cBullet, MAX_BULLETS> m_values{};
};