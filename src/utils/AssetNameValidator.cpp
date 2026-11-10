/**
 * @file AssetNameValidator.cpp
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

#include "AssetNameValidator.h"

#include "utils/Log.h"

std::vector<AssetMismatch> AssetNameValidator::validate(std::span<const AssetEntry> entries, std::span<const std::string> packNames)
{
    std::vector<AssetMismatch> mismatches;
    for (const AssetEntry &entry : entries) {
        if (entry.index < 0 || static_cast<size_t>(entry.index) >= packNames.size()) {
            mismatches.push_back({std::string(entry.name), entry.index, "", true});
        } else if (packNames[entry.index] != entry.name) {
            mismatches.push_back({std::string(entry.name), entry.index, packNames[entry.index], false});
        }
    }
    return mismatches;
}

int AssetNameValidator::validateAndLog(std::string_view packLabel, std::span<const AssetEntry> entries, std::span<const std::string> packNames)
{
    const auto mismatches = validate(entries, packNames);
    for (const AssetMismatch &m : mismatches) {
        if (m.indexOutOfRange) {
            Logger::warn(COMP_INIT, "Asset validation", "{}: {} expects index {} but the pack has only {} entries", packLabel, m.expectedName, m.index, packNames.size());
        } else {
            Logger::warn(COMP_INIT, "Asset validation", "{}: {} expects index {} but the pack has '{}' there", packLabel, m.expectedName, m.index, m.actualName);
        }
    }
    if (mismatches.empty()) {
        Logger::info(COMP_INIT, "Asset validation", "{}: all {} names match their index", packLabel, entries.size());
    } else {
        Logger::warn(COMP_INIT, "Asset validation", "{}: {} of {} names do not match their index", packLabel, mismatches.size(), entries.size());
    }
    return static_cast<int>(mismatches.size());
}
