/**
 * @file KillDeathRatio.hpp
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

#include <format>
#include <string>

// Kill-death ratio (units destroyed / units lost) as text with one decimal, e.g. "6.0".
// With no units lost the number of kills is shown instead; with no kills and no losses "-".
inline std::string killDeathRatioText(int unitsDestroyed, int unitsLost)
{
    if (unitsLost == 0 && unitsDestroyed == 0) return "-";
    if (unitsLost == 0) return std::format("{:.1f}", static_cast<double>(unitsDestroyed));
    return std::format("{:.1f}", static_cast<double>(unitsDestroyed) / unitsLost);
}
