#pragma once

#include <array>
#include <cassert>
#include <cstddef>
#include "definitions.h"
#include "gameobjects/projectiles/bullet.h"

/**
 * Container wrapper for the global g_Bullet array
 * Provides safe indexed access and standard container interface
 */
class cBullets {
public:
    cBullets() = default;

    cBullet &operator[](std::size_t index) {
        assert(index < MAX_BULLETS && "Index out of bounds for g_Bullet");
        return m_values[index];
    }

    const cBullet &operator[](std::size_t index) const {
        assert(index < MAX_BULLETS && "Index out of bounds for g_Bullet");
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