/**
 * @file StructureInfosLoader.hpp
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

class cStructureInfos;
class Texture;
struct SDL_Surface;

// Fills the structure info table from structures.ini: every section is a structure, named by the section ([WOR]).
// Structures without a section keep the default values. Bitmap, shadow, flash and icon names are resolved through the given lookups.
class StructureInfosLoader {
public:
    using SurfaceLookup = std::function<SDL_Surface*(std::string_view)>;
    using TextureLookup = std::function<Texture*(std::string_view)>;
    using IconLookup = std::function<TextureID(std::string_view)>;

    StructureInfosLoader(SurfaceLookup surfaceLookup, TextureLookup textureLookup, IconLookup iconLookup);

    // Returns false when the file could not be opened (all structures then keep the default values)
    bool load(const std::string& filename, cStructureInfos& structureInfos) const;

private:
    SurfaceLookup m_surfaceLookup;
    TextureLookup m_textureLookup;
    IconLookup m_iconLookup;
};
