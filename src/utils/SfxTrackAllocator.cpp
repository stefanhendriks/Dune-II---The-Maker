#include "SfxTrackAllocator.h"

int pickSfxTrack(const std::vector<bool> &trackBusy, int roundRobinIndex)
{
    const int n = static_cast<int>(trackBusy.size());
    for (int offset = 0; offset < n; ++offset) {
        int idx = (roundRobinIndex + offset) % n;
        if (!trackBusy[idx]) {
            return idx;
        }
    }
    return roundRobinIndex % n;
}
