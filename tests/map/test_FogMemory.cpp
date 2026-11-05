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

static s_RememberedStructure structureSlice(int type, int offsetX, int offsetY)
{
    s_RememberedStructure s;
    s.type = type;
    s.ownerId = 2;
    s.frame = 1;
    s.offsetX = offsetX;
    s.offsetY = offsetY;
    return s;
}

TEST_CASE("FogMemory - structure on a cell dropping out of sight is remembered", "[map][fog]")
{
    FogMemory memory(16);
    memory.updateStructure(3, true, false, structureSlice(7, 1, 0));

    const s_RememberedStructure *remembered = memory.getStructure(3);
    REQUIRE(remembered != nullptr);
    REQUIRE(remembered->type == 7);
    REQUIRE(remembered->ownerId == 2);
    REQUIRE(remembered->frame == 1);
    REQUIRE(remembered->offsetX == 1);
    REQUIRE(remembered->offsetY == 0);
}

TEST_CASE("FogMemory - structure destroyed or built while fogged does not change the memory", "[map][fog]")
{
    FogMemory memory(16);
    memory.updateStructure(3, true, false, structureSlice(7, 0, 0));
    // destroyed while fogged: live has no structure anymore, memory stays
    memory.updateStructure(3, false, false, std::nullopt);
    REQUIRE(memory.getStructure(3) != nullptr);

    // built while fogged on a cell without memory: nothing shows up
    memory.updateStructure(4, false, false, structureSlice(7, 0, 0));
    REQUIRE(memory.getStructure(4) == nullptr);
}

TEST_CASE("FogMemory - empty cell dropping out of sight remembers no structure", "[map][fog]")
{
    FogMemory memory(16);
    memory.updateStructure(3, true, false, std::nullopt);
    REQUIRE(memory.getStructure(3) == nullptr);
}

TEST_CASE("FogMemory - seeing the cell again forgets the structure", "[map][fog]")
{
    FogMemory memory(16);
    memory.updateStructure(3, true, false, structureSlice(7, 0, 0));
    memory.updateStructure(3, false, true, std::nullopt);
    REQUIRE(memory.getStructure(3) == nullptr);
}

TEST_CASE("FogMemory - out of range structure cells are ignored", "[map][fog]")
{
    FogMemory memory(16);
    memory.updateStructure(-1, true, false, structureSlice(7, 0, 0));
    memory.updateStructure(16, true, false, structureSlice(7, 0, 0));
    REQUIRE(memory.getStructure(-1) == nullptr);
    REQUIRE(memory.getStructure(16) == nullptr);
}

static s_RememberedUnit tank(int type, int bodyIndex, int headIndex)
{
    s_RememberedUnit u;
    u.type = type;
    u.bodyIndex = bodyIndex;
    u.headIndex = headIndex;
    u.frame = 0;
    return u;
}

TEST_CASE("FogMemory - unit on a cell dropping out of sight is remembered with hull and turret", "[map][fog]")
{
    FogMemory memory(16);
    memory.updateUnit(3, true, false, tank(9, 2, 5));

    const s_RememberedUnit *remembered = memory.getUnit(3);
    REQUIRE(remembered != nullptr);
    REQUIRE(remembered->type == 9);
    REQUIRE(remembered->bodyIndex == 2);
    REQUIRE(remembered->headIndex == 5);
}

TEST_CASE("FogMemory - unit stays at its last seen cell while fogged", "[map][fog]")
{
    FogMemory memory(16);
    memory.updateUnit(3, true, false, tank(9, 2, 5));
    // unit drove away / arrived elsewhere while fogged: memory of both cells is unchanged
    memory.updateUnit(3, false, false, std::nullopt);
    memory.updateUnit(4, false, false, tank(9, 1, 1));

    REQUIRE(memory.getUnit(3) != nullptr);
    REQUIRE(memory.getUnit(4) == nullptr);
}

TEST_CASE("FogMemory - empty cell dropping out of sight remembers no unit", "[map][fog]")
{
    FogMemory memory(16);
    memory.updateUnit(3, true, false, std::nullopt);
    REQUIRE(memory.getUnit(3) == nullptr);
}

TEST_CASE("FogMemory - seeing the cell again forgets the unit", "[map][fog]")
{
    FogMemory memory(16);
    memory.updateUnit(3, true, false, tank(9, 2, 5));
    memory.updateUnit(3, false, true, std::nullopt);
    REQUIRE(memory.getUnit(3) == nullptr);
}

TEST_CASE("FogMemory - out of range unit cells are ignored", "[map][fog]")
{
    FogMemory memory(16);
    memory.updateUnit(-1, true, false, tank(9, 2, 5));
    memory.updateUnit(16, true, false, tank(9, 2, 5));
    REQUIRE(memory.getUnit(-1) == nullptr);
    REQUIRE(memory.getUnit(16) == nullptr);
}
