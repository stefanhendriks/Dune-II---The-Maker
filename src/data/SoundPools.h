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

#include "data/gfxaudio.h"
#include "utils/SoundPool.hpp"

#include <array>

// Variant sounds by name. Replaces `SOUND_X + rnd(n)` index math on gfxaudio.h.
namespace soundpools {

namespace ids {
inline constexpr std::array musicWin = {MIDI_WIN01, MIDI_WIN02, MIDI_WIN03};
inline constexpr std::array musicLose = {MIDI_LOSE01, MIDI_LOSE02, MIDI_LOSE03};
inline constexpr std::array musicAttack = {MIDI_ATTACK01, MIDI_ATTACK02, MIDI_ATTACK03, MIDI_ATTACK04, MIDI_ATTACK05, MIDI_ATTACK06};
inline constexpr std::array musicPeace = {MIDI_BUILDING01, MIDI_BUILDING02, MIDI_BUILDING03, MIDI_BUILDING04, MIDI_BUILDING05,
                                          MIDI_BUILDING06, MIDI_BUILDING07, MIDI_BUILDING08, MIDI_BUILDING09};
inline constexpr std::array infantryAcknowledge = {SOUND_MOVINGOUT, SOUND_MOVINGOUT2};
inline constexpr std::array vehicleAcknowledge = {SOUND_ACKNOWLEDGED, SOUND_AFFIRMATIVE, SOUND_AFFIRMATIVE2};
inline constexpr std::array infantryDie = {SOUND_DIE01, SOUND_DIE02, SOUND_DIE03, SOUND_DIE04, SOUND_DIE05};
inline constexpr std::array tankDie = {SOUND_TANKDIE, SOUND_TANKDIE2};
inline constexpr std::array structureCrumble = {SOUND_CRUMBLE01, SOUND_CRUMBLE02};
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

} // namespace soundpools
