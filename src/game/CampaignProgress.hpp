/**
 * @file CampaignProgress.hpp
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

#include <array>
#include <cstddef>
#include <filesystem>
#include <string>

enum class CampaignProgressLoadResult {
    NotFound,
    Loaded,
    Invalid
};

class CampaignProgress {
public:
    static constexpr int FIRST_MISSION = 1;
    static constexpr int LAST_MISSION = 9;
    static constexpr int COMPLETED = LAST_MISSION + 1;

    explicit CampaignProgress(std::filesystem::path filePath);

    CampaignProgressLoadResult load(std::string& error);
    bool save(std::string& error) const;

    int nextMission(int house) const;
    int highestUnlockedMission(int house) const;
    static bool supportsHouse(int house);
    bool markStarted(int house);
    bool recordMissionVictory(int house, int mission);

private:
    static std::size_t houseIndex(int house);

    std::filesystem::path m_filePath;
    std::array<int, 4> m_nextMission{};
};
