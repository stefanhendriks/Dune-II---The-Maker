#pragma once

#include <vector>

// Picks which pooled sound-effect track should play the next sound.
int pickSfxTrack(const std::vector<bool> &trackBusy, int roundRobinIndex);
