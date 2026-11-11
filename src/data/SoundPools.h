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

// Variant sounds by name. Replaces `SOUND_X + rnd(n)` index math.
namespace soundpools {

// Pack names, as stored in the audio pack (sdl_audio.dat). Static storage: SoundPool views the ids below.
namespace names {
inline constexpr std::array<std::string_view, 3> musicWin = {"MIDI_WIN01", "MIDI_WIN02", "MIDI_WIN03"};
inline constexpr std::array<std::string_view, 3> musicLose = {"MIDI_LOSE01", "MIDI_LOSE02", "MIDI_LOSE03"};
inline constexpr std::array<std::string_view, 6> musicAttack = {"MIDI_ATTACK01", "MIDI_ATTACK02", "MIDI_ATTACK03",
                                                                "MIDI_ATTACK04", "MIDI_ATTACK05", "MIDI_ATTACK06"};
inline constexpr std::array<std::string_view, 9> musicPeace = {"MIDI_BUILDING01", "MIDI_BUILDING02", "MIDI_BUILDING03",
                                                               "MIDI_BUILDING04", "MIDI_BUILDING05", "MIDI_BUILDING06",
                                                               "MIDI_BUILDING07", "MIDI_BUILDING08", "MIDI_BUILDING09"};
inline constexpr std::array<std::string_view, 2> infantryAcknowledge = {"SOUND_MOVINGOUT", "SOUND_MOVINGOUT2"};
inline constexpr std::array<std::string_view, 3> vehicleAcknowledge = {"SOUND_ACKNOWLEDGED", "SOUND_AFFIRMATIVE", "SOUND_AFFIRMATIVE2"};
inline constexpr std::array<std::string_view, 5> infantryDie = {"SOUND_DIE01", "SOUND_DIE02", "SOUND_DIE03", "SOUND_DIE04", "SOUND_DIE05"};
inline constexpr std::array<std::string_view, 2> tankDie = {"SOUND_TANKDIE1", "SOUND_TANKDIE2"};
inline constexpr std::array<std::string_view, 2> structureCrumble = {"SOUND_CRUMBLE01", "SOUND_CRUMBLE02"};
// Voice lines: one entry per house, in the order Atreides, Harkonnen, Ordos (see AudioContext::playVoice).
inline constexpr std::array<std::string_view, 3> voice01 = {"SOUND_VOICE_01_ATR", "SOUND_VOICE_01_HAR", "SOUND_VOICE_01_ORD"};
inline constexpr std::array<std::string_view, 3> voice02 = {"SOUND_VOICE_02_ATR", "SOUND_VOICE_02_HAR", "SOUND_VOICE_02_ORD"};
inline constexpr std::array<std::string_view, 3> voice03 = {"SOUND_VOICE_03_ATR", "SOUND_VOICE_03_HAR", "SOUND_VOICE_03_ORD"};
inline constexpr std::array<std::string_view, 3> voice04 = {"SOUND_VOICE_04_ATR", "SOUND_VOICE_04_HAR", "SOUND_VOICE_04_ORD"};
inline constexpr std::array<std::string_view, 3> voice05 = {"SOUND_VOICE_05_ATR", "SOUND_VOICE_05_HAR", "SOUND_VOICE_05_ORD"};
inline constexpr std::array<std::string_view, 3> voice06 = {"SOUND_VOICE_06_ATR", "SOUND_VOICE_06_HAR", "SOUND_VOICE_06_ORD"};
inline constexpr std::array<std::string_view, 3> voice07 = {"SOUND_VOICE_07_ATR", "SOUND_VOICE_07_HAR", "SOUND_VOICE_07_ORD"};
inline constexpr std::array<std::string_view, 3> voice08 = {"SOUND_VOICE_08_ATR", "SOUND_VOICE_08_HAR", "SOUND_VOICE_08_ORD"};
inline constexpr std::array<std::string_view, 3> voice09 = {"SOUND_VOICE_09_ATR", "SOUND_VOICE_09_HAR", "SOUND_VOICE_09_ORD"};
inline constexpr std::array<std::string_view, 3> voice10 = {"SOUND_VOICE_10_ATR", "SOUND_VOICE_10_HAR", "SOUND_VOICE_10_ORD"};
inline constexpr std::array<std::string_view, 5> countdownAtreides = {"SOUND_ATR_S1", "SOUND_ATR_S2", "SOUND_ATR_S3", "SOUND_ATR_S4", "SOUND_ATR_S5"};
inline constexpr std::array<std::string_view, 5> countdownHarkonnen = {"SOUND_HAR_S1", "SOUND_HAR_S2", "SOUND_HAR_S3", "SOUND_HAR_S4", "SOUND_HAR_S5"};
inline constexpr std::array<std::string_view, 5> countdownOrdos = {"SOUND_ORD_S1", "SOUND_ORD_S2", "SOUND_ORD_S3", "SOUND_ORD_S4", "SOUND_ORD_S5"};
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
inline std::array<int, 3> voice01{};
inline std::array<int, 3> voice02{};
inline std::array<int, 3> voice03{};
inline std::array<int, 3> voice04{};
inline std::array<int, 3> voice05{};
inline std::array<int, 3> voice06{};
inline std::array<int, 3> voice07{};
inline std::array<int, 3> voice08{};
inline std::array<int, 3> voice09{};
inline std::array<int, 3> voice10{};
inline std::array<int, 5> countdownAtreides{};
inline std::array<int, 5> countdownHarkonnen{};
inline std::array<int, 5> countdownOrdos{};
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
inline constexpr SoundPool voice01{ids::voice01};
inline constexpr SoundPool voice02{ids::voice02};
inline constexpr SoundPool voice03{ids::voice03};
inline constexpr SoundPool voice04{ids::voice04};
inline constexpr SoundPool voice05{ids::voice05};
inline constexpr SoundPool voice06{ids::voice06};
inline constexpr SoundPool voice07{ids::voice07};
inline constexpr SoundPool voice08{ids::voice08};
inline constexpr SoundPool voice09{ids::voice09};
inline constexpr SoundPool voice10{ids::voice10};
inline constexpr SoundPool countdownAtreides{ids::countdownAtreides};
inline constexpr SoundPool countdownHarkonnen{ids::countdownHarkonnen};
inline constexpr SoundPool countdownOrdos{ids::countdownOrdos};

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
    missing += resolveSoundIds(names::voice01, ids::voice01, lookup);
    missing += resolveSoundIds(names::voice02, ids::voice02, lookup);
    missing += resolveSoundIds(names::voice03, ids::voice03, lookup);
    missing += resolveSoundIds(names::voice04, ids::voice04, lookup);
    missing += resolveSoundIds(names::voice05, ids::voice05, lookup);
    missing += resolveSoundIds(names::voice06, ids::voice06, lookup);
    missing += resolveSoundIds(names::voice07, ids::voice07, lookup);
    missing += resolveSoundIds(names::voice08, ids::voice08, lookup);
    missing += resolveSoundIds(names::voice09, ids::voice09, lookup);
    missing += resolveSoundIds(names::voice10, ids::voice10, lookup);
    missing += resolveSoundIds(names::countdownAtreides, ids::countdownAtreides, lookup);
    missing += resolveSoundIds(names::countdownHarkonnen, ids::countdownHarkonnen, lookup);
    missing += resolveSoundIds(names::countdownOrdos, ids::countdownOrdos, lookup);
    return missing;
}

} // namespace soundpools
