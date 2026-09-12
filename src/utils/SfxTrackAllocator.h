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
