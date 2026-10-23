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
#include "utils/cIniFile.h"

#include <algorithm>
#include <charconv>
#include <stdexcept>
#include <utility>

namespace {
constexpr std::size_t HOUSE_COUNT = 4;
constexpr std::array<const char*, HOUSE_COUNT> HOUSE_SECTIONS = {"Atreides", "Harkonnen", "Ordos", "Sardaukar"};
constexpr const char* MISSION_KEY = "mission";
constexpr int SAVE_VERSION = 1;
constexpr const char* SAVE_SIGNATURE = "D2TM_CAMPAIGN_PROGRESS";
constexpr const char* GENERAL_SECTION = "general";

// cIniFile expects UTF-8 file names, path::string() would give the ANSI code page on Windows
std::string utf8FileName(const std::filesystem::path& path)
{
    const std::u8string name = path.u8string();
    return std::string(name.begin(), name.end());
}
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

    cIniFile ini;
    if (!ini.load(utf8FileName(m_filePath))) {
        error = "Could not open campaign progress file";
        return CampaignProgressLoadResult::Invalid;
    }

    // Keep the unusable file as .old so the next save does not overwrite it
    const auto rejectFile = [&](const std::string& reason) {
        error = reason;
        std::error_code renameError;
        std::filesystem::path backupPath = m_filePath;
        backupPath.replace_extension(".old");
        std::filesystem::rename(m_filePath, backupPath, renameError);
        if (renameError) {
            error += " (could not rename it to " + backupPath.string() + ": " + renameError.message() + ")";
        } else {
            error += " (renamed to " + backupPath.string() + ")";
        }
        return CampaignProgressLoadResult::Invalid;
    };

    if (!ini.hasSection(GENERAL_SECTION)) {
        return rejectFile("Campaign progress file has no general section");
    }
    const cSection general = ini.getSection(GENERAL_SECTION);
    if (general.getStringValue("SAVE_SIGNATURE") != SAVE_SIGNATURE || general.getInt("SAVE_VERSION") != SAVE_VERSION) {
        return rejectFile("Campaign progress file has an invalid signature or version");
    }

    // A missing section or key means no progress yet, so items can be added to the file later on
    std::array<int, HOUSE_COUNT> loadedProgress{};
    for (std::size_t i = 0; i < HOUSE_COUNT; i++) {
        if (!ini.hasSection(HOUSE_SECTIONS[i])) continue;
        const cSection section = ini.getSection(HOUSE_SECTIONS[i]);
        if (!section.hasValue(MISSION_KEY)) continue;

        const std::string value = section.getStringValue(MISSION_KEY);
        int mission = 0;
        const auto [end, ec] = std::from_chars(value.data(), value.data() + value.size(), mission);
        if (ec != std::errc() || end != value.data() + value.size() || mission < 0 || mission > COMPLETED) {
            return rejectFile("Campaign progress file contains an invalid mission number");
        }
        loadedProgress[i] = mission;
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

    cIniFile ini;
    ini.setValue(GENERAL_SECTION, "SAVE_SIGNATURE", SAVE_SIGNATURE);
    ini.setValue(GENERAL_SECTION, "SAVE_VERSION", std::to_string(SAVE_VERSION));
    for (std::size_t i = 0; i < HOUSE_COUNT; i++) {
        ini.setValue(HOUSE_SECTIONS[i], MISSION_KEY, std::to_string(m_nextMission[i]));
    }

    if (!ini.save(utf8FileName(m_filePath))) {
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
    return house == ATREIDES || house == HARKONNEN || house == ORDOS || house == SARDAUKAR;
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
        case SARDAUKAR:
            return 3;
        default:
            throw std::invalid_argument("Campaign progress is not supported for this house");
    }
}
