/**
 * @file UpgradeInfosLoader.hpp
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

class cUpgradeInfos;

// Fills the upgrade table from upgrades.ini: every section is an upgrade, named by the section
// (for example [HEAVYFCTRY_MVC], the name of the UPGRADE_TYPE_ constant without that prefix).
// Upgrades without a section stay disabled. A key that is absent keeps its default.
// Icon names and the costs of structures and units are resolved through the given lookups,
// so the structure and unit rules must be loaded before this one.
class UpgradeInfosLoader {
public:
    using IconLookup = std::function<TextureID(std::string_view)>;
    using CostLookup = std::function<int(int)>; // type id -> cost

    UpgradeInfosLoader(IconLookup iconLookup, CostLookup structureCost, CostLookup unitCost, bool cheatMode);

    // Returns false when the file could not be opened (all upgrades then stay disabled)
    bool load(const std::string& filename, cUpgradeInfos& upgradeInfos) const;

private:
    IconLookup m_iconLookup;
    CostLookup m_structureCost;
    CostLookup m_unitCost;
    bool m_cheatMode;
};
