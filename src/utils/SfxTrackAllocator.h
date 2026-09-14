/**
 * @file SfxTrackAllocator.h
 *
 * Dune 2 - The Maker
 *
 * @author Stefan Hendriks & the D2TM Team
 * @www http://www.dune2themaker.com
 * @copyright Copyright (c) 2001 - 2026 D2TM Team
 * @license This software is released under the MIT License.
 *
 * Note: Dune 2 is a trademark of Westwood Studios/Electronic Arts.
 *
 * This is a non-commercial educational project.
 */

#pragma once

#include <vector>

// Picks which pooled sound-effect track should play a new sound.
//
// Prefers a track that has finished playing. If every track is busy, steals
// one already playing a sound no more important than newSoundPriority
// (higher number = higher priority), so e.g. voice dialogue is not silently
// replaced by a repeating economy tick. If every busy track is playing a
// strictly higher-priority sound, falls back to stealing the round-robin
// position, since something has to give.
int pickSfxTrack(const std::vector<bool> &trackBusy, const std::vector<int> &trackPriority, int newSoundPriority, int roundRobinIndex);
