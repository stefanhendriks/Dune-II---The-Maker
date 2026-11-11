/**
 * @file SoundPools.h
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

#include "utils/SoundPool.hpp"

#include <array>
#include <functional>
#include <string_view>

// Variant sounds by name. Replaces `SOUND_X + rnd(n)` index math on gfxaudio.h.
namespace soundpools {

// Pack names, as stored in the audio pack (sdl_audio.dat). Static storage: SoundPool views the ids below.
namespace names {
inline constexpr std::array<std::string_view, 3> musicWin = {"MIDI_WIN01", "MIDI_WIN02", "MIDI_WIN03"};
inline constexpr std::array<std::string_view, 3> musicLose = {"MIDI_LOSE1", "MIDI_LOSE2", "MIDI_LOSE3"};
inline constexpr std::array<std::string_view, 6> musicAttack = {"MIDI_ATTACK01", "MIDI_ATTACK02", "MIDI_ATTACK03",
                                                                "MIDI_ATTACK04", "MIDI_ATTACK05", "MIDI_ATTACK06"};
inline constexpr std::array<std::string_view, 9> musicPeace = {"MIDI_BUILDING01", "MIDI_BUILDING02", "MIDI_BUILDING03",
                                                               "MIDI_BUILDING04", "MIDI_BUILDING05", "MIDI_BUILDING06",
                                                               "MIDI_BUILDING07", "MIDI_BUILDING08", "MIDI_BUILDING09"};
inline constexpr std::array<std::string_view, 2> infantryAcknowledge = {"SOUND_MOVINGOUT", "SOUND_MOVINGOUT2"};
inline constexpr std::array<std::string_view, 3> vehicleAcknowledge = {"SOUND_ACKNOWLEDGED", "SOUND_AFFIRMATIVE", "SOUND_AFFIRMATIVE2"};
inline constexpr std::array<std::string_view, 5> infantryDie = {"SOUND_DIE01", "SOUND_DIE02", "SOUND_DIE03", "SOUND_DIE04", "SOUND_DIE05"};
// "SOUND_TANK" (not "SOUND_TANKDIE2") is what gfxaudio.h's SOUND_TANKDIE2 index points at in the pack.
inline constexpr std::array<std::string_view, 2> tankDie = {"SOUND_TANKDIE", "SOUND_TANK"};
inline constexpr std::array<std::string_view, 2> structureCrumble = {"SOUND_CRUMBLE01", "SOUND_CRUMBLE02"};
} // namespace names

// Pack indices. Written only once, by resolve() from the AudioContext constructor; read-only afterwards.
namespace ids {
inline std::array<int, 3> musicWin{};
inline std::array<int, 3> musicLose{};
inline std::array<int, 6> musicAttack{};
inline std::array<int, 9> musicPeace{};
inline std::array<int, 2> infantryAcknowledge{};
inline std::array<int, 3> vehicleAcknowledge{};
inline std::array<int, 5> infantryDie{};
inline std::array<int, 2> tankDie{};
inline std::array<int, 2> structureCrumble{};
} // namespace ids

inline constexpr SoundPool musicWin{ids::musicWin};
inline constexpr SoundPool musicLose{ids::musicLose};
inline constexpr SoundPool musicAttack{ids::musicAttack};
inline constexpr SoundPool musicPeace{ids::musicPeace};
inline constexpr SoundPool infantryAcknowledge{ids::infantryAcknowledge};
inline constexpr SoundPool vehicleAcknowledge{ids::vehicleAcknowledge};
inline constexpr SoundPool infantryDie{ids::infantryDie};
inline constexpr SoundPool tankDie{ids::tankDie};
inline constexpr SoundPool structureCrumble{ids::structureCrumble};

// Looks every pool name up in the audio pack. Returns how many names were not found.
inline int resolve(const std::function<int(std::string_view)> &lookup) {
    int missing = 0;
    missing += resolveSoundIds(names::musicWin, ids::musicWin, lookup);
    missing += resolveSoundIds(names::musicLose, ids::musicLose, lookup);
    missing += resolveSoundIds(names::musicAttack, ids::musicAttack, lookup);
    missing += resolveSoundIds(names::musicPeace, ids::musicPeace, lookup);
    missing += resolveSoundIds(names::infantryAcknowledge, ids::infantryAcknowledge, lookup);
    missing += resolveSoundIds(names::vehicleAcknowledge, ids::vehicleAcknowledge, lookup);
    missing += resolveSoundIds(names::infantryDie, ids::infantryDie, lookup);
    missing += resolveSoundIds(names::tankDie, ids::tankDie, lookup);
    missing += resolveSoundIds(names::structureCrumble, ids::structureCrumble, lookup);
    return missing;
}

} // namespace soundpools
