/**
 * @file MusicJukeBox.cpp
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

#include "MusicJukeBox.hpp"

#include "utils/RNG.hpp"

#include <algorithm>
#include <cctype>
#include <string>
#include <system_error>
#include <vector>

namespace {
bool isAudioFile(const std::filesystem::path &path)
{
    std::string ext = path.extension().string();
    std::ranges::transform(ext, ext.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });
    return ext == ".ogg" || ext == ".mp3" || ext == ".wav" || ext == ".flac";
}

// empty when there is no directory for this type (or house, for briefing music)
std::filesystem::path repertoryDir(eMusicType type, int house)
{
    switch (type) {
    case eMusicType::WIN: return "win";
    case eMusicType::LOSE: return "lose";
    case eMusicType::ATTACK: return "attack";
    case eMusicType::PEACE: return "peace";
    case eMusicType::MENU: return "menu";
    case eMusicType::CONQUEST: return "conquest";
    case eMusicType::BRIEFING:
        switch (house) {
        case ATREIDES: return "briefing/atreides";
        case HARKONNEN: return "briefing/harkonnen";
        case ORDOS: return "briefing/ordos";
        case SARDAUKAR: return "briefing/sardaukar";
        default: return "";
        }
    }
    return "";
}

std::vector<std::filesystem::path> listAudioFiles(const std::filesystem::path &dir)
{
    std::vector<std::filesystem::path> files;
    std::error_code ec;
    for (std::filesystem::directory_iterator it(dir, ec), end; !ec && it != end; it.increment(ec)) {
        if (it->is_regular_file(ec) && isAudioFile(it->path())) {
            files.push_back(it->path());
        }
    }
    return files;
}
} // namespace

MusicJukeBox::MusicJukeBox(std::filesystem::path root) :
    m_root(std::move(root))
{
}

void MusicJukeBox::createRepertoryDirectories() const
{
    std::error_code ec;
    for (const char *dir : {"win", "lose", "attack", "peace", "menu", "conquest",
                            "briefing/atreides", "briefing/harkonnen", "briefing/ordos", "briefing/sardaukar"}) {
        std::filesystem::create_directories(m_root / dir, ec);
    }
}

void MusicJukeBox::setCustomMusicEnabled(bool enabled)
{
    m_customMusicEnabled = enabled;
}

MusicChoice MusicJukeBox::choose(eMusicType type, int archiveTrack, int house) const
{
    MusicChoice choice{archiveTrack, std::nullopt};
    if (!m_customMusicEnabled) {
        return choice;
    }

    const std::filesystem::path subDir = repertoryDir(type, house);
    if (subDir.empty()) {
        return choice;
    }

    std::vector<std::filesystem::path> files = listAudioFiles(m_root / subDir);
    if (files.empty()) {
        return choice;
    }

    // directory order is unspecified, sort so the pick does not depend on it
    std::ranges::sort(files);
    choice.file = files[RNG::rnd(static_cast<int>(files.size()))];
    return choice;
}
