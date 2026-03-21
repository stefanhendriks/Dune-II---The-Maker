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
