#include <catch2/catch_test_macros.hpp>
#include "utils/SfxTrackAllocator.h"

TEST_CASE("pickSfxTrack - prefers a free track over stealing a busy one", "[audio]")
{
    // Round robin would land on index 0 next, but it is still playing;
    // index 2 has already finished and should be reused instead of stopping
    // whatever is still playing on index 0.
    std::vector<bool> trackBusy = {true, true, false, true};
    REQUIRE(pickSfxTrack(trackBusy, 0) == 2);
}

TEST_CASE("pickSfxTrack - wraps around the pool when searching for a free track", "[audio]")
{
    std::vector<bool> trackBusy = {false, true, true, true};
    REQUIRE(pickSfxTrack(trackBusy, 3) == 0);
}

TEST_CASE("pickSfxTrack - all tracks free picks the round robin position", "[audio]")
{
    std::vector<bool> trackBusy = {false, false, false, false};
    REQUIRE(pickSfxTrack(trackBusy, 2) == 2);
}

TEST_CASE("pickSfxTrack - all tracks busy falls back to the round robin position", "[audio]")
{
    std::vector<bool> trackBusy = {true, true, true, true};
    REQUIRE(pickSfxTrack(trackBusy, 1) == 1);
}
