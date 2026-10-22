/**
 * @file CampaignProgress.cpp
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

#include "game/CampaignProgress.hpp"

#include "include/definitions.h"

#include <algorithm>
#include <fstream>
#include <stdexcept>
#include <utility>

namespace {
constexpr int SAVE_VERSION = 1;
constexpr const char* SAVE_SIGNATURE = "D2TM_CAMPAIGN_PROGRESS";
}

CampaignProgress::CampaignProgress(std::filesystem::path filePath) :
    m_filePath(std::move(filePath))
{
}

CampaignProgressLoadResult CampaignProgress::load(std::string& error)
{
    std::error_code filesystemError;
    const bool fileExists = std::filesystem::exists(m_filePath, filesystemError);
    if (filesystemError) {
        error = "Could not check campaign progress file: " + filesystemError.message();
        return CampaignProgressLoadResult::Invalid;
    }
    if (!fileExists) {
        m_nextMission.fill(0);
        return CampaignProgressLoadResult::NotFound;
    }

    std::ifstream input(m_filePath);
    if (!input.is_open()) {
        error = "Could not open campaign progress file";
        return CampaignProgressLoadResult::Invalid;
    }

    std::string signature;
    int version = 0;
    std::array<int, 3> loadedProgress{};
    if (!(input >> signature >> version >> loadedProgress[0] >> loadedProgress[1] >> loadedProgress[2])) {
        error = "Campaign progress file is incomplete";
        return CampaignProgressLoadResult::Invalid;
    }

    input >> std::ws;
    if (!input.eof() || signature != SAVE_SIGNATURE || version != SAVE_VERSION) {
        error = "Campaign progress file has an invalid signature, version, or trailing data";
        return CampaignProgressLoadResult::Invalid;
    }

    if (std::any_of(loadedProgress.begin(), loadedProgress.end(), [](int mission) {
            return mission < 0 || mission > COMPLETED;
        })) {
        error = "Campaign progress file contains an invalid mission number";
        return CampaignProgressLoadResult::Invalid;
    }

    m_nextMission = loadedProgress;
    return CampaignProgressLoadResult::Loaded;
}

bool CampaignProgress::save(std::string& error) const
{
    std::error_code filesystemError;
    std::filesystem::create_directories(m_filePath.parent_path(), filesystemError);
    if (filesystemError) {
        error = "Could not create campaign progress directory: " + filesystemError.message();
        return false;
    }

    std::ofstream output(m_filePath, std::ios::trunc);
    if (!output.is_open()) {
        error = "Could not open campaign progress file for writing";
        return false;
    }

    output << SAVE_SIGNATURE << ' ' << SAVE_VERSION << '\n'
           << m_nextMission[0] << ' ' << m_nextMission[1] << ' ' << m_nextMission[2] << '\n';
    output.flush();
    if (!output) {
        error = "Could not write campaign progress file";
        return false;
    }
    return true;
}

int CampaignProgress::nextMission(int house) const
{
    return m_nextMission[houseIndex(house)];
}

int CampaignProgress::highestUnlockedMission(int house) const
{
    const int next = nextMission(house);
    return std::min(next, LAST_MISSION);
}

bool CampaignProgress::supportsHouse(int house)
{
    return house == ATREIDES || house == HARKONNEN || house == ORDOS;
}

bool CampaignProgress::markStarted(int house)
{
    int& next = m_nextMission[houseIndex(house)];
    if (next != 0) {
        return false;
    }

    next = FIRST_MISSION;
    return true;
}

bool CampaignProgress::recordMissionVictory(int house, int mission)
{
    if (mission < FIRST_MISSION || mission > LAST_MISSION) {
        throw std::invalid_argument("Campaign mission is outside the valid range");
    }

    int& next = m_nextMission[houseIndex(house)];
    const int nextAfterVictory = mission + 1;
    if (next >= nextAfterVictory) {
        return false;
    }

    next = nextAfterVictory;
    return true;
}

std::size_t CampaignProgress::houseIndex(int house)
{
    switch (house) {
        case ATREIDES:
            return 0;
        case HARKONNEN:
            return 1;
        case ORDOS:
            return 2;
        default:
            throw std::invalid_argument("Campaign progress is only supported for the three main houses");
    }
}
