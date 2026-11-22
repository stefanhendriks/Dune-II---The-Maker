/**
 * @file cParticleInfos.h
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

#include "utils/GraphicsID.hpp"
#include <array>
#include <cstddef>
#include <stdexcept>

// Particle drawing layers (z-index like) to draw particle
enum class RenderLayerParticle : char {
    TOP =0,     // top (over unit)
    BOTTOM = 1  // bottom (beneath unit)
};

/**
 * Particle info
 * A particle can have 3 states:
 * - fade in
 * - animating
 * - fade out (either while animating or not)
 */

// A particle that is spawned together with another one
struct s_ParticleSpawn {
    int type;           // particle type (D2TM_PARTICLE_*), -1 = none
    int offsetX;        // offset to the position of the spawning particle
    int offsetY;
    bool inheritHouse;  // true: uses the house colors of the spawning particle
};

constexpr int MAX_PARTICLE_SPAWNS = 3;

struct s_ParticleInfo {
    TextureID bmpIndex;           // index of bmp to draw (index in DataRepository)
    int startAlpha;         // 0-255 (anything else == 255 = opaque)
    bool usesAdditiveBlending; // type of blending used when alpha > 0 and < 255 (default = transparent, if this is set to false)
    RenderLayerParticle layer;  // at which 'layer' (z-index like) to draw particle (0 = top (over unit), 1 = bottom (beneath unit))
    int frameWidth;         // width of frame
    int frameHeight;        // height of frame
    bool oldParticle;       // if true, then keep old behavior for now (will be removed)

    // values used when the particle is spawned
    int spawnFrameTimer;        // frame timer at spawn (ticks of 5 ms)
    int spawnFrameTimerRandom;  // when > 0: a random 0 .. n-1 is added to spawnFrameTimer
    int spawnDeadTimer;         // time (ticks of 5 ms) the particle lives before it starts to die, 0 = starts dying at once
    int spawnDeadTimerRandom;   // when > 0: a random 0 .. n-1 is added to spawnDeadTimer
    int startAlphaRandom;       // when > 0: a random 0 .. n-1 is added to startAlpha
    bool anchorBottom;          // true: the position is the bottom of the bitmap instead of its center
    s_ParticleSpawn spawns[MAX_PARTICLE_SPAWNS]; // particles that are spawned together with this one

    // values used while the particle animates and fades (timers are in ticks of 5 ms)
    int frameDelay;             // time between animation steps
    int frameDelayRandom;       // when > 0: a random 0 .. n-1 is added to frameDelay
    int fadeDelay;              // time between steps while fading out, -1 = same as frameDelay
    int lastAnimFrame;          // last frame of the animation, after it the particle fades out or dies
    int holdFrame;              // frame that is shown while fading out
    int frameCount;             // number of frames of an animation that loops
    int fadeStep;               // alpha that is subtracted per fade step
    int fadeInStep;             // alpha that is added per step while fading in
    int fadeInMaxAlpha;         // alpha at which fading in stops
    int dieAlpha;               // the particle dies when it fades below this alpha
    int nextFrameBelowAlpha;    // alpha below which the second frame is shown
    int wearChance;             // chance in percent per step that the alpha drops by 1 (a track wears off)
    int wearMinAlpha;           // alpha above which a particle can wear off

    int getFadeDelay() const { return fadeDelay < 0 ? frameDelay : fadeDelay; }
};


class cParticleInfos {
public:
    cParticleInfos()
    {}

    // accès
    s_ParticleInfo &operator[](std::size_t i) {
        if (i >= MAX_PARTICLE_TYPES) throw std::out_of_range("Particle index");
        return m_data[i];
    }

    const s_ParticleInfo &operator[](std::size_t i) const {
        if (i >= MAX_PARTICLE_TYPES) throw std::out_of_range("Particle index");
        return m_data[i];
    }

    // getter explicite
    s_ParticleInfo &get(std::size_t i) { return (*this)[i]; }
    const s_ParticleInfo &get(std::size_t i) const { return (*this)[i]; }

    // taille
    static constexpr std::size_t size() noexcept { return MAX_PARTICLE_TYPES; }

    // recherche/existence
    bool valid(std::size_t i) const noexcept { return i < MAX_PARTICLE_TYPES; }

    // initialisation
    void reset() noexcept {
        for (auto &p : m_data) {
            p = s_ParticleInfo{}; // ou init spécifique
        }
    }

    // itérateurs
    auto begin() noexcept { return m_data.begin(); }
    auto end() noexcept { return m_data.end(); }
    auto begin() const noexcept { return m_data.begin(); }
    auto end() const noexcept { return m_data.end(); }

private:
    static constexpr std::size_t MAX_PARTICLE_TYPES = 50;
    std::array<s_ParticleInfo, MAX_PARTICLE_TYPES> m_data;
};