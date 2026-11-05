#include <catch2/catch_test_macros.hpp>
#include "gameobjects/map/FogMemory.hpp"

static s_RememberedTerrain terrain(int type, int tile)
{
    s_RememberedTerrain t;
    t.type = type;
    t.tile = tile;
    return t;
}

TEST_CASE("FogMemory - nothing remembered initially", "[map][fog]")
{
    FogMemory memory(16);
    REQUIRE(memory.getTerrain(3) == nullptr);
}

TEST_CASE("FogMemory - cell dropping out of sight is remembered", "[map][fog]")
{
    FogMemory memory(16);
    memory.update(3, true, false, terrain(2, 5));

    const s_RememberedTerrain *remembered = memory.getTerrain(3);
    REQUIRE(remembered != nullptr);
    REQUIRE(remembered->type == 2);
    REQUIRE(remembered->tile == 5);
}

TEST_CASE("FogMemory - remembered smudge is kept", "[map][fog]")
{
    FogMemory memory(16);
    s_RememberedTerrain live = terrain(1, 0);
    live.smudgetile = 4;
    live.smudgetype = SmudgeType::S_ROCK;
    memory.update(3, true, false, live);

    const s_RememberedTerrain *remembered = memory.getTerrain(3);
    REQUIRE(remembered != nullptr);
    REQUIRE(remembered->smudgetile == 4);
    REQUIRE(remembered->smudgetype == SmudgeType::S_ROCK);
}

TEST_CASE("FogMemory - fogged cell keeps its first memory when live terrain changes", "[map][fog]")
{
    FogMemory memory(16);
    memory.update(3, true, false, terrain(2, 5));
    // still unseen on later updates: live terrain changed (spice harvested), memory must not
    memory.update(3, false, false, terrain(1, 0));

    REQUIRE(memory.getTerrain(3)->type == 2);
    REQUIRE(memory.getTerrain(3)->tile == 5);
}

TEST_CASE("FogMemory - seeing the cell again forgets the memory", "[map][fog]")
{
    FogMemory memory(16);
    memory.update(3, true, false, terrain(2, 5));
    memory.update(3, false, true, terrain(1, 0));

    REQUIRE(memory.getTerrain(3) == nullptr);
}

TEST_CASE("FogMemory - never seen cell has no memory", "[map][fog]")
{
    FogMemory memory(16);
    memory.update(3, false, false, terrain(2, 5));

    REQUIRE(memory.getTerrain(3) == nullptr);
}

TEST_CASE("FogMemory - out of range cells are ignored", "[map][fog]")
{
    FogMemory memory(16);
    memory.update(-1, true, false, terrain(2, 5));
    memory.update(16, true, false, terrain(2, 5));

    REQUIRE(memory.getTerrain(-1) == nullptr);
    REQUIRE(memory.getTerrain(16) == nullptr);
}
