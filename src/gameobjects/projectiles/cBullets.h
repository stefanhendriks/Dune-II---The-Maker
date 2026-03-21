#pragma once

#include <array>
#include <cassert>
#include "definitions.h"
#include "gameobjects/projectiles/bullet.h"

/**
 * Container wrapper for the global g_Bullet array
 * Provides safe indexed access and standard container interface
 */
class cBullets {
public:
    using value_type = cBullet;
    using size_type = std::size_t;

    cBullets() = default;

    value_type &operator[](size_type index) {
        assert(index < MAX_BULLETS && "Index out of bounds for g_Bullet");
        return m_values[index];
    }

    const value_type &operator[](size_type index) const {
        assert(index < MAX_BULLETS && "Index out of bounds for g_Bullet");
        return m_values[index];
    }

    value_type *data() noexcept { return m_values.data(); }
    const value_type *data() const noexcept { return m_values.data(); }

    size_type size() const noexcept { return m_values.size(); }

    auto begin() noexcept { return m_values.begin(); }
    auto end() noexcept { return m_values.end(); }
    auto begin() const noexcept { return m_values.begin(); }
    auto end() const noexcept { return m_values.end(); }

private:
    static constexpr std::size_t MAX_BULLETS = 300;     // max of bullets in the game at once
    std::array<value_type, MAX_BULLETS> m_values{};
};
