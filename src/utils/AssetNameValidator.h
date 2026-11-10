/**
 * @file AssetNameValidator.h
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

#include <span>
#include <string>
#include <string_view>
#include <vector>

// A constant from one of the gfx*.h headers: its name (= expected pack name) and the index it claims.
struct AssetEntry {
    std::string_view name;
    int index;
};

struct AssetMismatch {
    std::string expectedName;
    int index;
    std::string actualName; // empty when the index is outside the pack
    bool indexOutOfRange;
};

class AssetNameValidator {
public:
    // Compares every entry with the name stored in the pack at that index (packNames[i] = name of file i).
    static std::vector<AssetMismatch> validate(std::span<const AssetEntry> entries, std::span<const std::string> packNames);

    // validate() + one log line per mismatch and a summary line.
    // Returns the number of mismatches.
    static int validateAndLog(std::string_view packLabel, std::span<const AssetEntry> entries, std::span<const std::string> packNames);
};
