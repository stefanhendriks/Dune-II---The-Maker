#pragma once

#include <array>
#include <cstddef>
#include <stdexcept>

constexpr std::size_t MAX_PARTICLE_TYPES = 50;

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

struct s_ParticleInfo {
    int bmpIndex;           // index of bmp to draw (index in DataRepository)
    int startAlpha;         // 0-255 (anything else == 255 = opaque)
    bool usesAdditiveBlending; // type of blending used when alpha > 0 and < 255 (default = transparent, if this is set to false)
    RenderLayerParticle layer;  // at which 'layer' (z-index like) to draw particle (0 = top (over unit), 1 = bottom (beneath unit))
    int frameWidth;         // width of frame
    int frameHeight;        // height of frame
    bool oldParticle;       // if true, then keep old behavior for now (will be removed)
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
    std::array<s_ParticleInfo, MAX_PARTICLE_TYPES> m_data;
};