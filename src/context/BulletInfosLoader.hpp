/**
 * @file BulletInfosLoader.hpp
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

class cBulletInfos;
class Texture;

// Fills the bullet info table from bullets.ini: every section is a bullet, named by the section ([ROCKET_NORMAL]).
// Bullets without a section keep the default values. Bitmap and sound names are resolved through the given lookups.
class BulletInfosLoader {
public:
    using TextureLookup = std::function<Texture*(std::string_view)>;
    using SoundLookup = std::function<audioID(std::string_view)>;

    BulletInfosLoader(TextureLookup textureLookup, SoundLookup soundLookup);

    // Returns false when the file could not be opened (all bullets then keep the default values)
    bool load(const std::string& filename, cBulletInfos& bulletInfos) const;

private:
    TextureLookup m_textureLookup;
    SoundLookup m_soundLookup;
};
