/**
 * @file MusicJukeBox.hpp
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

#include "definitions.h"

#include <filesystem>
#include <optional>

// What to play for a music request: always an archive track (the fallback),
// plus a file from the repertory directory when one was picked.
struct MusicChoice {
    int archiveTrack = 0;
    std::optional<std::filesystem::path> file;
};

// Decides what music to play; it does not play anything itself.
// ATTACK and PEACE may pick a file from "<root>/attack" or "<root>/peace",
// but only when custom music is enabled and the directory holds audio files.
class MusicJukeBox {
public:
    explicit MusicJukeBox(std::filesystem::path root = "audio");

    void setCustomMusicEnabled(bool enabled);

    [[nodiscard]] MusicChoice choose(eMusicType type, int archiveTrack) const;

private:
    std::filesystem::path m_root;
    bool m_customMusicEnabled = false;
};
