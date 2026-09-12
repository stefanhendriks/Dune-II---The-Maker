#include <catch2/catch_test_macros.hpp>
#include "utils/SfxTrackAllocator.h"

namespace {
    constexpr int kNormal = 0;
    constexpr int kHigh = 1;
}

TEST_CASE("pickSfxTrack - prefers a free track over stealing a busy one", "[audio]")
{
    // Round robin would land on index 0 next, but it is still playing;
    // index 2 has already finished and should be reused instead of stopping
    // whatever is still playing on index 0.
    std::vector<bool> trackBusy = {true, true, false, true};
    std::vector<int> trackPriority = {kNormal, kNormal, kNormal, kNormal};
    REQUIRE(pickSfxTrack(trackBusy, trackPriority, kNormal, 0) == 2);
}

TEST_CASE("pickSfxTrack - wraps around the pool when searching for a free track", "[audio]")
{
    std::vector<bool> trackBusy = {false, true, true, true};
    std::vector<int> trackPriority = {kNormal, kNormal, kNormal, kNormal};
    REQUIRE(pickSfxTrack(trackBusy, trackPriority, kNormal, 3) == 0);
}

TEST_CASE("pickSfxTrack - all tracks free picks the round robin position", "[audio]")
{
    std::vector<bool> trackBusy = {false, false, false, false};
    std::vector<int> trackPriority = {kNormal, kNormal, kNormal, kNormal};
    REQUIRE(pickSfxTrack(trackBusy, trackPriority, kNormal, 2) == 2);
}

TEST_CASE("pickSfxTrack - all tracks busy, same priority, falls back to round robin position", "[audio]")
{
    std::vector<bool> trackBusy = {true, true, true, true};
    std::vector<int> trackPriority = {kNormal, kNormal, kNormal, kNormal};
    REQUIRE(pickSfxTrack(trackBusy, trackPriority, kNormal, 1) == 1);
}

TEST_CASE("pickSfxTrack - no free track steals a lower-priority busy one instead of the round robin position", "[audio]")
{
    // Round robin would land on index 0 (playing a high-priority voice line),
    // but index 2 is a lower-priority sound and should be stolen instead.
    std::vector<bool> trackBusy = {true, true, true, true};
    std::vector<int> trackPriority = {kHigh, kHigh, kNormal, kHigh};
    REQUIRE(pickSfxTrack(trackBusy, trackPriority, kNormal, 0) == 2);
}

TEST_CASE("pickSfxTrack - a high-priority sound does not spare an equal-priority track", "[audio]")
{
    // Nothing is strictly lower priority than the incoming high-priority
    // sound, so there is nothing to protect; fall back to round robin.
    std::vector<bool> trackBusy = {true, true, true, true};
    std::vector<int> trackPriority = {kHigh, kHigh, kHigh, kHigh};
    REQUIRE(pickSfxTrack(trackBusy, trackPriority, kHigh, 2) == 2);
}

TEST_CASE("pickSfxTrack - low-priority sound cannot steal from a high-priority track", "[audio]")
{
    std::vector<bool> trackBusy = {true, true, true, true};
    std::vector<int> trackPriority = {kHigh, kHigh, kHigh, kHigh};
    // Nothing lower priority than kNormal exists among kHigh tracks, so this
    // also falls back to the round robin position.
    REQUIRE(pickSfxTrack(trackBusy, trackPriority, kNormal, 3) == 3);
}
