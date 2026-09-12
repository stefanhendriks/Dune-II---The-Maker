#include "SfxTrackAllocator.h"

int pickSfxTrack(const std::vector<bool> &trackBusy, const std::vector<int> &trackPriority, int newSoundPriority, int roundRobinIndex)
{
    const int n = static_cast<int>(trackBusy.size());

    for (int offset = 0; offset < n; ++offset) {
        int idx = (roundRobinIndex + offset) % n;
        if (!trackBusy[idx]) {
            return idx;
        }
    }

    for (int offset = 0; offset < n; ++offset) {
        int idx = (roundRobinIndex + offset) % n;
        if (trackPriority[idx] <= newSoundPriority) {
            return idx;
        }
    }

    return roundRobinIndex % n;
}
