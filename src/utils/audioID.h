/**
 * @file audioID.h
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

// Identifies one entry (sound or music) in the audio pack.
// Resolved from a name once, at init, by AudioContext::indexOf(); everything that
// plays audio takes it as is, there is no conversion or arithmetic at play time.
// -1 means "no audio" (unknown name, or nothing to play).
using audioID = int;

inline constexpr audioID noAudioID = -1;
