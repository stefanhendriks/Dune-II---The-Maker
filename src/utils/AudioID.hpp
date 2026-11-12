/**
 * @file AudioID.hpp
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

#include <cstdint>

// Identifies an entry (sound or music) in the audio pack, as returned by AudioContext::indexOf().
// The values are pack indices; `invalid` is the "not found / no sound" value (the uint32 wrap of -1).
enum class audioID : std::uint32_t {
    invalid = 0xFFFFFFFFu
};

constexpr audioID toAudioID(int packIndex)
{
    return static_cast<audioID>(static_cast<std::uint32_t>(packIndex));
}

// the pack index of the sound, or -1 for audioID::invalid
constexpr int toPackIndex(audioID id)
{
    return static_cast<int>(static_cast<std::uint32_t>(id));
}
