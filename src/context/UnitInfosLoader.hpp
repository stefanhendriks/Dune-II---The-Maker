/**
 * @file UnitInfosLoader.hpp
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

#include "utils/AudioID.hpp"
#include "utils/GraphicsID.hpp"

class cUnitInfos;
class Texture;
struct SDL_Surface;

// Fills the unit info table from units.ini: every section is a unit, named by the section ([TRIKE]).
// Units without a section keep the default values. Bitmap, shadow, icon and sound names are resolved through the given lookups.
class UnitInfosLoader {
public:
    using SurfaceLookup = std::function<SDL_Surface*(std::string_view)>;
    using TextureLookup = std::function<Texture*(std::string_view)>;
    using IconLookup = std::function<TextureID(std::string_view)>;
    using SoundLookup = std::function<audioID(std::string_view)>;

    UnitInfosLoader(SurfaceLookup surfaceLookup, TextureLookup textureLookup, IconLookup iconLookup, SoundLookup soundLookup);

    // Returns false when the file could not be opened (all units then keep the default values)
    bool load(const std::string& filename, cUnitInfos& unitInfos) const;

private:
    SurfaceLookup m_surfaceLookup;
    TextureLookup m_textureLookup;
    IconLookup m_iconLookup;
    SoundLookup m_soundLookup;
};
