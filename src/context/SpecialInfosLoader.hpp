/**
 * @file SpecialInfosLoader.hpp
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

class cSpecialInfos;

// Fills the special weapon (superweapon) table from specials.ini: every section is a special,
// named by the section: [SABOTEUR], [FREMEN] or [DEATHHAND].
// Specials without a section keep the default values (which means nobody can build them).
// A key that is absent keeps its default. Icon names are resolved through the given lookup.
class SpecialInfosLoader {
public:
    using IconLookup = std::function<TextureID(std::string_view)>;

    explicit SpecialInfosLoader(IconLookup iconLookup);

    // Returns false when the file could not be opened (all specials then keep the default values)
    bool load(const std::string& filename, cSpecialInfos& specialInfos) const;

private:
    IconLookup m_iconLookup;
};
