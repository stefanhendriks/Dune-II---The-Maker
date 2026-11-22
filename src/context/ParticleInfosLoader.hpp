/**
 * @file ParticleInfosLoader.hpp
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

#include <functional>
#include <string>
#include <string_view>

#include "utils/GraphicsID.hpp"

class cParticleInfos;

// Fills the particle table from particles.ini: every section is a particle, named like the
// D2TM_PARTICLE_ constant without that prefix (for example [EXPLOSION_TRIKE]).
// Particles without a section have no bitmap. A key that is absent keeps its default.
class ParticleInfosLoader {
public:
    using TextureLookup = std::function<TextureID(std::string_view)>;

    explicit ParticleInfosLoader(TextureLookup textureLookup);

    // Returns false when the file could not be opened (all particles then keep their defaults)
    bool load(const std::string& filename, cParticleInfos& particleInfos) const;

private:
    TextureLookup m_textureLookup;
};
