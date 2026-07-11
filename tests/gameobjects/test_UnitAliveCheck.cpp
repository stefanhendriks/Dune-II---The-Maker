#include <catch2/catch_test_macros.hpp>
#include "gameobjects/units/UnitAliveCheck.hpp"

// unitCountsAsAliveForDefeat(isDead, isCarried)
//   isDead    : cUnit::isDead()    → iHitPoints <= 0
//   isCarried : cUnit::isCarried() → iTempHitPoints > -1 (inside carryall or structure)

TEST_CASE("unitCountsAsAliveForDefeat - normal alive unit", "[unit][alive-check]")
{
    REQUIRE(unitCountsAsAliveForDefeat(false, false) == true);
}

TEST_CASE("unitCountsAsAliveForDefeat - unit in carryall or structure counts as alive", "[unit][alive-check]")
{
    // iHitPoints = -1 (isDead), but iTempHitPoints > 0 (isCarried) — being transported
    REQUIRE(unitCountsAsAliveForDefeat(true, true) == true);
}

TEST_CASE("unitCountsAsAliveForDefeat - truly dead unit does not count", "[unit][alive-check]")
{
    REQUIRE(unitCountsAsAliveForDefeat(true, false) == false);
}

TEST_CASE("unitCountsAsAliveForDefeat - alive unit that is also carried counts as alive", "[unit][alive-check]")
{
    // Edge case: isDead=false, isCarried=true should never happen during normal gameplay
    // but the predicate should still return true (not dead → alive).
    REQUIRE(unitCountsAsAliveForDefeat(false, true) == true);
}
