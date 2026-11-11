/**
 * @file Sounds.h
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
#include <functional>
#include <string_view>

// Single sounds by name. Each id is the index of the named entry in the audio pack (sdl_audio.dat),
// written only once by resolve() from the AudioContext constructor and read-only afterwards.
// -1 means "not resolved / not in the pack": playing it is silently skipped.
namespace sounds {

inline int menuMusic = -1;
inline int scenarioMusic = -1;
inline int mentatMusicAtreides = -1;
inline int mentatMusicHarkonnen = -1;
inline int mentatMusicOrdos = -1;
inline int reporting = -1;
inline int yesSir = -1;
inline int place = -1;
inline int radar = -1;
inline int button = -1;
inline int creditUp = -1;
inline int creditDown = -1;
inline int atreides = -1;
inline int harkonnen = -1;
inline int ordos = -1;
inline int rocket = -1;
inline int rocketSmall = -1;
inline int gun = -1;
inline int machineGun = -1;
inline int explosionRocket = -1;
inline int gunTurret = -1;
inline int shimmer = -1;
inline int gas = -1;
inline int squish = -1;
inline int trikeDie = -1;
inline int worm = -1;
inline int tankDie1 = -1;
inline int tankDie2 = -1;

struct Entry {
    std::string_view name;
    int *id;
};

inline const std::array<Entry, 28> entries = {{
    {"MIDI_MENU", &menuMusic},
    {"MIDI_SCENARIO", &scenarioMusic},
    {"MIDI_MENTAT_ATR", &mentatMusicAtreides},
    {"MIDI_MENTAT_HAR", &mentatMusicHarkonnen},
    {"MIDI_MENTAT_ORD", &mentatMusicOrdos},
    {"SOUND_REPORTING", &reporting},
    {"SOUND_YESSIR", &yesSir},
    {"SOUND_PLACE", &place},
    {"SOUND_RADAR", &radar},
    {"SOUND_BUTTON", &button},
    {"SOUND_CREDIT_UP", &creditUp},
    {"SOUND_CREDIT_DOWN", &creditDown},
    {"SOUND_ATREIDES", &atreides},
    {"SOUND_HARKONNEN", &harkonnen},
    {"SOUND_ORDOS", &ordos},
    {"SOUND_ROCKET", &rocket},
    {"SOUND_ROCKET_SMALL", &rocketSmall},
    {"SOUND_GUN", &gun},
    {"SOUND_MACHINEGUN", &machineGun},
    {"SOUND_EXPL_ROCKET", &explosionRocket},
    {"SOUND_GUNTURRET", &gunTurret},
    {"SOUND_SHIMMER", &shimmer},
    {"SOUND_GAS", &gas},
    {"SOUND_SQUISH", &squish},
    {"SOUND_TRIKEDIE", &trikeDie},
    {"SOUND_WORM", &worm},
    {"SOUND_TANKDIE1", &tankDie1},
    {"SOUND_TANKDIE2", &tankDie2},
}};

// Looks every sound name up in the audio pack. Returns how many names were not found.
inline int resolve(const std::function<int(std::string_view)> &lookup) {
    int missing = 0;
    for (const Entry &entry : entries) {
        *entry.id = lookup(entry.name);
        if (*entry.id < 0) missing++;
    }
    return missing;
}

} // namespace sounds
