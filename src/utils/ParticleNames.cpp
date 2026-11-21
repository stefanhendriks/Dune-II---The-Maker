/**
 * @file ParticleNames.cpp
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

#include "utils/ParticleNames.hpp"

#include <array>

#include "definitions.h"

namespace {

struct s_ParticleName {
    std::string_view name;
    int id;
};

constexpr std::array kParticleNames = std::to_array<s_ParticleName>({
    {"MOVE", D2TM_PARTICLE_MOVE},
    {"ATTACK", D2TM_PARTICLE_ATTACK},
    {"EXPLOSION_TRIKE", D2TM_PARTICLE_EXPLOSION_TRIKE},
    {"SMOKE", D2TM_PARTICLE_SMOKE},
    {"SMOKE_SHADOW", D2TM_PARTICLE_SMOKE_SHADOW},
    {"TRACK_DIA", D2TM_PARTICLE_TRACK_DIA},
    {"TRACK_HOR", D2TM_PARTICLE_TRACK_HOR},
    {"TRACK_VER", D2TM_PARTICLE_TRACK_VER},
    {"TRACK_DIA2", D2TM_PARTICLE_TRACK_DIA2},
    {"BULLET_PUF", D2TM_PARTICLE_BULLET_PUF},
    {"EXPLOSION_FIRE", D2TM_PARTICLE_EXPLOSION_FIRE},
    {"WORMEAT", D2TM_PARTICLE_WORMEAT},
    {"EXPLOSION_TANK_ONE", D2TM_PARTICLE_EXPLOSION_TANK_ONE},
    {"EXPLOSION_TANK_TWO", D2TM_PARTICLE_EXPLOSION_TANK_TWO},
    {"EXPLOSION_STRUCTURE01", D2TM_PARTICLE_EXPLOSION_STRUCTURE01},
    {"EXPLOSION_STRUCTURE02", D2TM_PARTICLE_EXPLOSION_STRUCTURE02},
    {"EXPLOSION_GAS", D2TM_PARTICLE_EXPLOSION_GAS},
    {"WORMTRAIL", D2TM_PARTICLE_WORMTRAIL},
    {"DEADINF01", D2TM_PARTICLE_DEADINF01},
    {"DEADINF02", D2TM_PARTICLE_DEADINF02},
    {"TANKSHOOT", D2TM_PARTICLE_TANKSHOOT},
    {"SIEGESHOOT", D2TM_PARTICLE_SIEGESHOOT},
    {"SQUISH01", D2TM_PARTICLE_SQUISH01},
    {"SQUISH02", D2TM_PARTICLE_SQUISH02},
    {"SQUISH03", D2TM_PARTICLE_SQUISH03},
    {"EXPLOSION_ORNI", D2TM_PARTICLE_EXPLOSION_ORNI},
    {"SIEGEDIE", D2TM_PARTICLE_SIEGEDIE},
    {"CARRYPUFF", D2TM_PARTICLE_CARRYPUFF},
    {"EXPLOSION_ROCKET", D2TM_PARTICLE_EXPLOSION_ROCKET},
    {"EXPLOSION_ROCKET_SMALL", D2TM_PARTICLE_EXPLOSION_ROCKET_SMALL},
    {"OBJECT_BOOM01", D2TM_PARTICLE_OBJECT_BOOM01},
    {"OBJECT_BOOM02", D2TM_PARTICLE_OBJECT_BOOM02},
    {"OBJECT_BOOM03", D2TM_PARTICLE_OBJECT_BOOM03},
    {"EXPLOSION_BULLET", D2TM_PARTICLE_EXPLOSION_BULLET},
    {"SMOKE_WITH_SHADOW", D2TM_PARTICLE_SMOKE_WITH_SHADOW},
    {"EXPLOSION_CARRYALL", D2TM_PARTICLE_EXPLOSION_CARRYALL},
});

} // namespace

int particleIdFromName(std::string_view name)
{
    for (const auto& entry : kParticleNames) {
        if (entry.name == name) return entry.id;
    }
    return -1;
}
