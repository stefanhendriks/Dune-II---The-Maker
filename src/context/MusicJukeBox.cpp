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

void MusicJukeBox::setCustomMusicEnabled(bool enabled)
{
    m_customMusicEnabled = enabled;
}

MusicChoice MusicJukeBox::choose(eMusicType type, int archiveTrack) const
{
    MusicChoice choice{archiveTrack, std::nullopt};
    if (!m_customMusicEnabled) {
        return choice;
    }

    if (type != eMusicType::ATTACK && type != eMusicType::PEACE) {
        return choice;
    }

    const char *subDir = (type == eMusicType::ATTACK) ? "attack" : "peace";
    std::vector<std::filesystem::path> files = listAudioFiles(m_root / subDir);
    if (files.empty()) {
        return choice;
    }

    // directory order is unspecified, sort so the pick does not depend on it
    std::ranges::sort(files);
    choice.file = files[RNG::rnd(static_cast<int>(files.size()))];
    return choice;
}
