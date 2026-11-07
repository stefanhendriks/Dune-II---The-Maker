#include <catch2/catch_test_macros.hpp>
#include "utils/LowPowerBlink.hpp"

TEST_CASE("LowPowerBlink::opacityAt - starts at the minimum", "[lowpowerblink]")
{
    REQUIRE(LowPowerBlink::opacityAt(0) == LowPowerBlink::MIN_OPACITY);
}

TEST_CASE("LowPowerBlink::opacityAt - peaks at half a period", "[lowpowerblink]")
{
    REQUIRE(LowPowerBlink::opacityAt(LowPowerBlink::PERIOD_MS / 2) == LowPowerBlink::MAX_OPACITY);
}

TEST_CASE("LowPowerBlink::opacityAt - repeats every period", "[lowpowerblink]")
{
    REQUIRE(LowPowerBlink::opacityAt(LowPowerBlink::PERIOD_MS) == LowPowerBlink::MIN_OPACITY);
    REQUIRE(LowPowerBlink::opacityAt(123) == LowPowerBlink::opacityAt(123 + 5 * LowPowerBlink::PERIOD_MS));
}

TEST_CASE("LowPowerBlink::opacityAt - is symmetric around the peak", "[lowpowerblink]")
{
    REQUIRE(LowPowerBlink::opacityAt(200) == LowPowerBlink::opacityAt(LowPowerBlink::PERIOD_MS - 200));
}

TEST_CASE("LowPowerBlink::opacityAt - stays within bounds", "[lowpowerblink]")
{
    for (std::uint64_t t = 0; t < 2 * LowPowerBlink::PERIOD_MS; t += 7) {
        int o = LowPowerBlink::opacityAt(t);
        REQUIRE(o >= LowPowerBlink::MIN_OPACITY);
        REQUIRE(o <= LowPowerBlink::MAX_OPACITY);
    }
}
