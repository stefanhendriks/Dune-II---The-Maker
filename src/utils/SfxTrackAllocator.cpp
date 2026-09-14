/**
 * @file SfxTrackAllocator.cpp
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
