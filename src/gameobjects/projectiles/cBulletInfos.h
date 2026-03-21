#pragma once

#include "enums.h"

#include <vector>
#include <string>
#include <array>

#include <SDL2/SDL.h>

class Texture;


struct s_BulletInfo {
    Texture *bmp;           // a reference to its bitmap. (16 bits here!)
    int deathParticle;      // when the bullet dies, it will use this particle to show its death
    int bmp_width;          // how many pixels a bullet is (wide)

    int damage_vehicles;    // how much damage it does to vehicles
    int damage_infantry;    // how much damage it does to infantry
    int explosionSize;      // square like explosion, defaults 1 (=1x1 tile), 2 means 2x2, etc.
    int smokeParticle;      // spawn smoke particles while traveling? (if > -1)
    int max_frames;         // when > 0 it animates automatically so many frames. If you want to flicker between frame 0 and 1, max_frames should be 1
    int max_deadframes;     // max dead frames

    int sound;              // Sound produced when 'shot' (or explosion sound)

    int deviateProbability; // how high probability it deviates a unit (changes sides) (0 = no chance, 100 = 100% chance)

    bool groundBullet;      // if true, then it gets blocked by walls, mountains or structures. False == flying bullets, ie, rockets
    bool canDamageAirUnits; // if true, then upon impact the bullet can also damage air units

    char description[64];   // ie: "bullet"

    bool canDamageGround;   // when true, this bullet can damage ground (ie walls, concrete, etc)
    int moveSpeed;
};


class cBulletInfos {
public:
    cBulletInfos() = default;

    s_BulletInfo &operator[](std::size_t index) {
        assert(index < MAX_BULLET_TYPES && "Index out of bounds for sBulletInfo");
        return m_values[index];
    }

    const s_BulletInfo &operator[](std::size_t index) const {
        assert(index < MAX_BULLET_TYPES && "Index out of bounds for sBulletInfo");
        return m_values[index];
    }

    s_BulletInfo *data() noexcept { return m_values.data(); }
    const s_BulletInfo *data() const noexcept { return m_values.data(); }

    std::size_t size() const noexcept { return m_values.size(); }

    auto begin() noexcept { return m_values.begin(); }
    auto end() noexcept { return m_values.end(); }
    auto begin() const noexcept { return m_values.begin(); }
    auto end() const noexcept { return m_values.end(); }

private:
    static constexpr std::size_t MAX_BULLET_TYPES = 30;
    std::array<s_BulletInfo, MAX_BULLET_TYPES> m_values{};
};