#include <catch2/catch_test_macros.hpp>

#include "utils/AssetNameValidator.h"
#include "data/AssetNameTables.h"

#include <array>
#include <string>
#include <vector>

TEST_CASE("AssetNameValidator reports nothing when names match their index", "[assets]")
{
    const std::vector<std::string> pack = {"A", "B", "C"};
    const std::array<AssetEntry, 2> entries = {{{"A", 0}, {"C", 2}}};
    CHECK(AssetNameValidator::validate(entries, pack).empty());
}

TEST_CASE("AssetNameValidator reports a swapped name", "[assets]")
{
    const std::vector<std::string> pack = {"SOUND_TANK", "SOUND_TANKDIE"};
    const std::array<AssetEntry, 1> entries = {{{"SOUND_TANKDIE", 0}}};
    const auto result = AssetNameValidator::validate(entries, pack);
    REQUIRE(result.size() == 1);
    CHECK(result[0].expectedName == "SOUND_TANKDIE");
    CHECK(result[0].actualName == "SOUND_TANK");
    CHECK_FALSE(result[0].indexOutOfRange);
}

TEST_CASE("AssetNameValidator reports an index outside the pack", "[assets]")
{
    const std::vector<std::string> pack = {"A"};
    const std::array<AssetEntry, 2> entries = {{{"B", 1}, {"C", -1}}};
    const auto result = AssetNameValidator::validate(entries, pack);
    REQUIRE(result.size() == 2);
    CHECK(result[0].indexOutOfRange);
    CHECK(result[1].indexOutOfRange);
}

TEST_CASE("Asset name tables pair every constant with its own name", "[assets]")
{
    REQUIRE_FALSE(assetnames::gfxdata.empty());
    CHECK(assetnames::gfxdata.front().name == "ATTACK_INDICATOR");
    CHECK(assetnames::gfxdata.front().index == ATTACK_INDICATOR);
    CHECK(assetnames::gfxaudio.front().name == "MIDI_ATTACK01");
    CHECK(assetnames::gfxinter.front().name == "BMP_D2TM");
}
