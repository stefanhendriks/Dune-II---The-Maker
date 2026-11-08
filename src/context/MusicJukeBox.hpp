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

#include "context/MusicChoice.hpp"
#include "definitions.h"

#include <filesystem>
#include <optional>

// Decides what music to play; it does not play anything itself.
// Every music type has its own directory below the root: win, lose, attack, peace, menu and conquest.
// BRIEFING has one per house: briefing/atreides, briefing/harkonnen, briefing/ordos and briefing/sardaukar.
// A file is only picked when custom music is enabled and the directory holds audio files.
class MusicJukeBox {
public:
    explicit MusicJukeBox(std::filesystem::path root = "audio");

    // Creates the empty directories players can put their music in; failure to do so is harmless.
    void createRepertoryDirectories() const;

    void setCustomMusicEnabled(bool enabled);

    [[nodiscard]] MusicChoice choose(eMusicType type, int archiveTrack, int house) const;

private:
    std::filesystem::path m_root;
    bool m_customMusicEnabled = false;
};
