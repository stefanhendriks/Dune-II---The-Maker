#include <catch2/catch_test_macros.hpp>

#include "definitions.h"
#include "utils/IniParsing.hpp"

#include <sstream>
#include <string>

namespace {

ScenarioData scenarioFrom(const std::string& text)
{
    std::istringstream stream(text);
    return parseScenario(stream);
}

template <typename T>
std::vector<T> entriesOfType(const ScenarioData& data)
{
    std::vector<T> result;
    for (const auto& entry : data.entries) {
        if (const auto* value = std::get_if<T>(&entry)) {
            result.push_back(*value);
        }
    }
    return result;
}

} // namespace

TEST_CASE("getHouseDirectoryName - known houses", "[ini_scenario]")
{
    REQUIRE(getHouseDirectoryName(ATREIDES) == "atreides");
    REQUIRE(getHouseDirectoryName(HARKONNEN) == "harkonnen");
    REQUIRE(getHouseDirectoryName(SARDAUKAR) == "sardaukar");
    REQUIRE(getHouseDirectoryName(ORDOS) == "ordos");
    REQUIRE(getHouseDirectoryName(FREMEN) == "fremen");
    REQUIRE(getHouseDirectoryName(MERCENARY) == "mercenary");
}

TEST_CASE("getHouseDirectoryName - unknown house", "[ini_scenario]")
{
    REQUIRE(getHouseDirectoryName(-1) == "unknown");
    REQUIRE(getHouseDirectoryName(GENERALHOUSE) == "unknown");
    REQUIRE(getHouseDirectoryName(999) == "unknown");
}

TEST_CASE("getBriefingFileName - one file per playable house", "[ini_scenario]")
{
    REQUIRE(getBriefingFileName(ATREIDES) == "mentata.ini");
    REQUIRE(getBriefingFileName(ORDOS) == "mentato.ini");
    REQUIRE(getBriefingFileName(HARKONNEN) == "mentath.ini");
    REQUIRE(getBriefingFileName(SARDAUKAR) == "mentats.ini");
    REQUIRE(getBriefingFileName(FREMEN).empty());
    REQUIRE(getBriefingFileName(-1).empty());
}

TEST_CASE("getTechLevelByRegion - region to mission mapping", "[ini_scenario]")
{
    REQUIRE(getTechLevelByRegion(1) == 1);
    REQUIRE(getTechLevelByRegion(2) == 2);
    REQUIRE(getTechLevelByRegion(4) == 2);
    REQUIRE(getTechLevelByRegion(5) == 3);
    REQUIRE(getTechLevelByRegion(7) == 3);
    REQUIRE(getTechLevelByRegion(8) == 4);
    REQUIRE(getTechLevelByRegion(10) == 4);
    REQUIRE(getTechLevelByRegion(11) == 5);
    REQUIRE(getTechLevelByRegion(13) == 5);
    REQUIRE(getTechLevelByRegion(14) == 6);
    REQUIRE(getTechLevelByRegion(16) == 6);
    REQUIRE(getTechLevelByRegion(17) == 7);
    REQUIRE(getTechLevelByRegion(19) == 7);
    REQUIRE(getTechLevelByRegion(20) == 8);
    REQUIRE(getTechLevelByRegion(21) == 8);
}

TEST_CASE("getTechLevelByRegion - anything past the last region is the final level", "[ini_scenario]")
{
    REQUIRE(getTechLevelByRegion(22) == 9);
    REQUIRE(getTechLevelByRegion(0) == 9);
    REQUIRE(getTechLevelByRegion(-5) == 9);
}

TEST_CASE("parseScenario - empty input gives default data", "[ini_scenario]")
{
    auto data = scenarioFrom("");
    REQUIRE(data.entries.empty());
    REQUIRE(data.mapLines.empty());
    REQUIRE(data.humanPlayerId == -1);
    REQUIRE_FALSE(data.briefScene.has_value());
    REQUIRE_FALSE(data.focusCell.has_value());
    REQUIRE_FALSE(data.winFlags.has_value());
    REQUIRE_FALSE(data.loseFlags.has_value());
    REQUIRE(data.playerHouse[0] == -1);
}

TEST_CASE("parseScenario - comments, blank lines and CRLF are ignored", "[ini_scenario]")
{
    auto data = scenarioFrom(
        "; a comment\r\n"
        "\r\n"
        "# another\r\n"
        "[BASIC]\r\n"
        "CursorPos=1234\r\n"
        "// trailing comment\r\n");
    REQUIRE(data.focusCell == 1234);
}

TEST_CASE("parseScenario - lines before the first known section are ignored", "[ini_scenario]")
{
    auto data = scenarioFrom(
        "CursorPos=99\n"
        "[BASIC]\n"
        "WinFlags=3\n");
    REQUIRE_FALSE(data.focusCell.has_value());
    REQUIRE(data.winFlags == 3);
}

TEST_CASE("parseScenario - unknown sections do not switch the current section", "[ini_scenario]")
{
    auto data = scenarioFrom(
        "[BASIC]\n"
        "[SOMETHINGELSE]\n"
        "CursorPos=77\n");
    REQUIRE(data.focusCell == 77);
}

TEST_CASE("parseScenario - BASIC section", "[ini_scenario]")
{
    auto data = scenarioFrom(
        "[BASIC]\n"
        "BriefPicture=HARVEST.WSA\n"
        "CursorPos=2080\n"
        "WinFlags=1\n"
        "LoseFlags=5\n");
    REQUIRE(data.briefScene == "harvest");
    REQUIRE(data.focusCell == 2080);
    REQUIRE(data.winFlags == 1);
    REQUIRE(data.loseFlags == 5);
}

TEST_CASE("parseScenario - unknown brief picture is not stored", "[ini_scenario]")
{
    auto data = scenarioFrom(
        "[BASIC]\n"
        "BriefPicture=NOPE.WSA\n");
    REQUIRE_FALSE(data.briefScene.has_value());
}

TEST_CASE("parseScenario - the last BASIC value wins", "[ini_scenario]")
{
    auto data = scenarioFrom(
        "[BASIC]\n"
        "WinFlags=1\n"
        "WinFlags=2\n");
    REQUIRE(data.winFlags == 2);
}

TEST_CASE("parseScenario - house sections get consecutive player ids", "[ini_scenario]")
{
    auto data = scenarioFrom(
        "[Atreides]\n"
        "Credits=2000\n"
        "Quota=500\n"
        "[Harkonnen]\n"
        "Credits=1500\n"
        "[Ordos]\n"
        "[Sardaukar]\n"
        "[Fremen]\n"
        "[Mercenary]\n");
    REQUIRE(data.playerHouse[0] == ATREIDES);
    REQUIRE(data.playerHouse[1] == HARKONNEN);
    REQUIRE(data.playerHouse[2] == ORDOS);
    REQUIRE(data.playerHouse[3] == SARDAUKAR);
    REQUIRE(data.playerHouse[4] == FREMEN);
    REQUIRE(data.playerHouse[5] == MERCENARY);
    REQUIRE(data.playerHouse[6] == -1);
}

TEST_CASE("parseScenario - credits are stored minus one, quota as is", "[ini_scenario]")
{
    auto data = scenarioFrom(
        "[Atreides]\n"
        "Credits=2000\n"
        "Quota=500\n"
        "[Harkonnen]\n"
        "Credits=1500\n");
    REQUIRE(data.playerCredits[0] == 1999);
    REQUIRE(data.playerQuota[0] == 500);
    REQUIRE(data.playerCredits[1] == 1499);
    REQUIRE(data.playerQuota[1] == 0);
}

TEST_CASE("parseScenario - the human brain is recorded case-insensitively", "[ini_scenario]")
{
    auto data = scenarioFrom(
        "[Atreides]\n"
        "Brain=CPU\n"
        "[Ordos]\n"
        "Brain=human\n");
    REQUIRE(data.humanPlayerId == 1);
}

TEST_CASE("parseScenario - no human brain leaves the human id unset", "[ini_scenario]")
{
    auto data = scenarioFrom(
        "[Atreides]\n"
        "Brain=CPU\n");
    REQUIRE(data.humanPlayerId == -1);
}

TEST_CASE("parseScenario - more house sections than players are clamped to the last slot", "[ini_scenario]")
{
    std::string text;
    for (int i = 0; i < MAX_PLAYERS + 2; i++) {
        text += "[Atreides]\nCredits=" + std::to_string(100 + i) + "\n";
    }
    auto data = scenarioFrom(text);
    REQUIRE(data.playerHouse[MAX_PLAYERS - 1] == ATREIDES);
    REQUIRE(data.playerCredits[MAX_PLAYERS - 1] == 100 + MAX_PLAYERS + 1 - 1);
}

TEST_CASE("parseScenario - SKIRMISH is not a house slot", "[ini_scenario]")
{
    auto data = scenarioFrom(
        "[SKIRMISH]\n"
        "StartCell=1234\n"
        "[Atreides]\n"
        "Credits=1000\n");
    REQUIRE(data.playerHouse[0] == ATREIDES);
    REQUIRE(data.playerCredits[0] == 999);
    REQUIRE(data.playerHouse[1] == -1);
}

TEST_CASE("parseScenario - MAP seed lines", "[ini_scenario]")
{
    auto data = scenarioFrom(
        "[MAP]\n"
        "Seed=4242\n");
    REQUIRE(data.mapLines.size() == 1);
    REQUIRE(data.mapLines[0] == 4242);
}

TEST_CASE("parseScenario - every MAP line is recorded, only Seed carries a value", "[ini_scenario]")
{
    auto data = scenarioFrom(
        "[MAP]\n"
        "Seed=7\n"
        "Bloom=1,2\n"
        "Field=3\n");
    REQUIRE(data.mapLines.size() == 3);
    REQUIRE(data.mapLines[0] == 7);
    REQUIRE_FALSE(data.mapLines[1].has_value());
    REQUIRE_FALSE(data.mapLines[2].has_value());
}

TEST_CASE("parseScenario - MAP bloom and field cells", "[ini_scenario]")
{
    auto data = scenarioFrom(
        "[MAP]\n"
        "Bloom=10, 20 ,30\n"
        "Field=100,200\n");
    REQUIRE(data.blooms[0] == 10);
    REQUIRE(data.blooms[1] == 20);
    REQUIRE(data.blooms[2] == 30);
    REQUIRE(data.blooms[3] == -1);
    REQUIRE(data.fields[0] == 100);
    REQUIRE(data.fields[1] == 200);
    REQUIRE(data.fields[2] == -1);
}

TEST_CASE("parseScenario - MAP cells skip empty tokens", "[ini_scenario]")
{
    auto data = scenarioFrom(
        "[MAP]\n"
        "Bloom=1,,2, ,3\n");
    REQUIRE(data.blooms[0] == 1);
    REQUIRE(data.blooms[1] == 2);
    REQUIRE(data.blooms[2] == 3);
    REQUIRE(data.blooms[3] == -1);
}

TEST_CASE("parseScenario - MAP cells beyond the capacity are dropped", "[ini_scenario]")
{
    std::string text = "[MAP]\nField=";
    for (int i = 0; i < ScenarioData::MAX_BLOOMS_AND_FIELDS + 5; i++) {
        text += std::to_string(i + 1) + ",";
    }
    text += "\n";
    auto data = scenarioFrom(text);
    REQUIRE(data.fields[0] == 1);
    REQUIRE(data.fields[ScenarioData::MAX_BLOOMS_AND_FIELDS - 1] == ScenarioData::MAX_BLOOMS_AND_FIELDS);
}

TEST_CASE("parseScenario - UNITS entries", "[ini_scenario]")
{
    auto data = scenarioFrom(
        "[UNITS]\n"
        "ID001=Atreides,Trike,256,1234,64,64\n"
        "ID002=Harkonnen, Quad ,256, 4321 ,64,64\n");
    auto units = entriesOfType<ScenarioData::Unit>(data);
    REQUIRE(units.size() == 2);

    REQUIRE(units[0].house == ATREIDES);
    REQUIRE(units[0].houseName == "Atreides");
    REQUIRE(units[0].type == TRIKE);
    REQUIRE(units[0].cell == 1234);

    REQUIRE(units[1].house == HARKONNEN);
    REQUIRE(units[1].houseName == "Harkonnen");
    REQUIRE(units[1].type == QUAD);
    REQUIRE(units[1].cell == 4321);
}

TEST_CASE("parseScenario - UNITS lines with missing fields keep defaults", "[ini_scenario]")
{
    auto data = scenarioFrom(
        "[UNITS]\n"
        "ID001=Atreides\n"
        "ID002=Atreides,Trike\n"
        "ID003=\n");
    auto units = entriesOfType<ScenarioData::Unit>(data);
    REQUIRE(units.size() == 3);

    REQUIRE(units[0].house == ATREIDES);
    REQUIRE(units[0].type == -1);
    REQUIRE(units[0].cell == -1);

    REQUIRE(units[1].type == TRIKE);
    REQUIRE(units[1].cell == -1);

    REQUIRE(units[2].houseName.empty());
}

TEST_CASE("parseScenario - UNITS with unknown house or unit resolve to -1", "[ini_scenario]")
{
    auto data = scenarioFrom(
        "[UNITS]\n"
        "ID001=Nobody,Spaceship,256,10,0,0\n");
    auto units = entriesOfType<ScenarioData::Unit>(data);
    REQUIRE(units.size() == 1);
    REQUIRE(units[0].house == -1);
    REQUIRE(units[0].type == -1);
    REQUIRE(units[0].cell == 10);
}

TEST_CASE("parseScenario - STRUCTURES entries", "[ini_scenario]")
{
    auto data = scenarioFrom(
        "[STRUCTURES]\n"
        "ID001=Atreides,Const Yard,256,2080\n"
        "ID002=Ordos,Refinery,200,100\n");
    auto structures = entriesOfType<ScenarioData::Structure>(data);
    REQUIRE(structures.size() == 2);

    REQUIRE_FALSE(structures[0].generated);
    REQUIRE(structures[0].house == ATREIDES);
    REQUIRE(structures[0].type == CONSTYARD);
    REQUIRE(structures[0].cell == 2080);

    REQUIRE(structures[1].house == ORDOS);
    REQUIRE(structures[1].type == REFINERY);
    REQUIRE(structures[1].cell == 100);
}

TEST_CASE("parseScenario - STRUCTURES with too few fields keep defaults", "[ini_scenario]")
{
    auto data = scenarioFrom(
        "[STRUCTURES]\n"
        "ID001=Atreides,Refinery\n");
    auto structures = entriesOfType<ScenarioData::Structure>(data);
    REQUIRE(structures.size() == 1);
    REQUIRE_FALSE(structures[0].house.has_value());
    REQUIRE(structures[0].type == -1);
    REQUIRE(structures[0].cell == -1);
}

TEST_CASE("parseScenario - GEN entries are walls and slabs", "[ini_scenario]")
{
    auto data = scenarioFrom(
        "[STRUCTURES]\n"
        "GEN0500=Atreides,Wall\n"
        "GEN0600=Harkonnen,Concrete\n"
        "GEN0700=Ordos,Something\n");
    auto structures = entriesOfType<ScenarioData::Structure>(data);
    REQUIRE(structures.size() == 3);

    REQUIRE(structures[0].generated);
    REQUIRE(structures[0].cell == 500);
    REQUIRE(structures[0].house == ATREIDES);
    REQUIRE(structures[0].type == WALL);

    REQUIRE(structures[1].generated);
    REQUIRE(structures[1].cell == 600);
    REQUIRE(structures[1].type == SLAB1);

    REQUIRE(structures[2].generated);
    REQUIRE(structures[2].type == -1);
}

TEST_CASE("parseScenario - GEN entry with only a house keeps defaults", "[ini_scenario]")
{
    auto data = scenarioFrom(
        "[STRUCTURES]\n"
        "GEN0500=Atreides\n");
    auto structures = entriesOfType<ScenarioData::Structure>(data);
    REQUIRE(structures.size() == 1);
    REQUIRE(structures[0].generated);
    REQUIRE(structures[0].cell == 500);
    REQUIRE_FALSE(structures[0].house.has_value());
}

TEST_CASE("parseScenario - REINFORCEMENTS to the home base", "[ini_scenario]")
{
    auto data = scenarioFrom(
        "[REINFORCEMENTS]\n"
        "1=Atreides,Quad,Homebase,5\n");
    auto reinforcements = entriesOfType<ScenarioData::Reinforcement>(data);
    REQUIRE(reinforcements.size() == 1);
    REQUIRE(reinforcements[0].house == ATREIDES);
    REQUIRE(reinforcements[0].unitType == QUAD);
    REQUIRE(reinforcements[0].location == ScenarioData::Reinforcement::Location::HOMEBASE);
    REQUIRE(reinforcements[0].delay == 5 * 20);
    REQUIRE_FALSE(reinforcements[0].repeat);
}

TEST_CASE("parseScenario - REINFORCEMENTS to the enemy base is case-insensitive", "[ini_scenario]")
{
    auto data = scenarioFrom(
        "[REINFORCEMENTS]\n"
        "1=Harkonnen,Trike,EnemyBase,2\n");
    auto reinforcements = entriesOfType<ScenarioData::Reinforcement>(data);
    REQUIRE(reinforcements.size() == 1);
    REQUIRE(reinforcements[0].location == ScenarioData::Reinforcement::Location::ENEMYBASE);
}

TEST_CASE("parseScenario - REINFORCEMENTS to a numeric cell", "[ini_scenario]")
{
    auto data = scenarioFrom(
        "[REINFORCEMENTS]\n"
        "1=Ordos,Trike, 1234 ,3\n");
    auto reinforcements = entriesOfType<ScenarioData::Reinforcement>(data);
    REQUIRE(reinforcements.size() == 1);
    REQUIRE(reinforcements[0].location == ScenarioData::Reinforcement::Location::CELL);
    REQUIRE(reinforcements[0].cell == 1234);
}

TEST_CASE("parseScenario - REINFORCEMENTS with a plus suffix repeat", "[ini_scenario]")
{
    auto data = scenarioFrom(
        "[REINFORCEMENTS]\n"
        "1=Ordos,Trike,Homebase,10+\n");
    auto reinforcements = entriesOfType<ScenarioData::Reinforcement>(data);
    REQUIRE(reinforcements.size() == 1);
    REQUIRE(reinforcements[0].repeat);
    REQUIRE(reinforcements[0].delay == 10 * 20);
}

TEST_CASE("parseScenario - REINFORCEMENTS with an invalid target is kept as INVALID", "[ini_scenario]")
{
    auto data = scenarioFrom(
        "[REINFORCEMENTS]\n"
        "1=Ordos,Trike,Nowhere,1\n"
        "2=Ordos,Trike,,1\n");
    auto reinforcements = entriesOfType<ScenarioData::Reinforcement>(data);
    REQUIRE(reinforcements.size() == 2);
    REQUIRE(reinforcements[0].location == ScenarioData::Reinforcement::Location::INVALID);
    REQUIRE(reinforcements[1].location == ScenarioData::Reinforcement::Location::INVALID);
}

TEST_CASE("parseScenario - REINFORCEMENTS with too few fields or no equals sign are skipped", "[ini_scenario]")
{
    auto data = scenarioFrom(
        "[REINFORCEMENTS]\n"
        "1=Ordos,Trike,Homebase\n"
        "garbage line\n");
    REQUIRE(data.entries.empty());
}

TEST_CASE("parseScenario - entries keep file order across sections", "[ini_scenario]")
{
    auto data = scenarioFrom(
        "[UNITS]\n"
        "ID001=Atreides,Trike,256,1,0,0\n"
        "[STRUCTURES]\n"
        "ID002=Atreides,Refinery,256,2\n"
        "[REINFORCEMENTS]\n"
        "1=Atreides,Quad,Homebase,1\n"
        "[UNITS]\n"
        "ID003=Atreides,Quad,256,3,0,0\n");
    REQUIRE(data.entries.size() == 4);
    REQUIRE(std::holds_alternative<ScenarioData::Unit>(data.entries[0]));
    REQUIRE(std::holds_alternative<ScenarioData::Structure>(data.entries[1]));
    REQUIRE(std::holds_alternative<ScenarioData::Reinforcement>(data.entries[2]));
    REQUIRE(std::holds_alternative<ScenarioData::Unit>(data.entries[3]));
}

TEST_CASE("parseScenario - a complete scenario", "[ini_scenario]")
{
    auto data = scenarioFrom(
        "[BASIC]\n"
        "BriefPicture=HARVEST.WSA\n"
        "CursorPos=2080\n"
        "[MAP]\n"
        "Seed=1\n"
        "Bloom=50\n"
        "[Atreides]\n"
        "Brain=Human\n"
        "Credits=2000\n"
        "[Harkonnen]\n"
        "Brain=CPU\n"
        "Credits=1000\n"
        "Quota=300\n"
        "[UNITS]\n"
        "ID001=Atreides,Trike,256,100,0,0\n"
        "[STRUCTURES]\n"
        "ID002=Atreides,Const Yard,256,200\n"
        "GEN0300=Harkonnen,Wall\n");
    REQUIRE(data.briefScene == "harvest");
    REQUIRE(data.focusCell == 2080);
    REQUIRE(data.humanPlayerId == 0);
    REQUIRE(data.playerCredits[0] == 1999);
    REQUIRE(data.playerCredits[1] == 999);
    REQUIRE(data.playerQuota[1] == 300);
    REQUIRE(data.blooms[0] == 50);
    REQUIRE(data.mapLines.size() == 2);
    REQUIRE(entriesOfType<ScenarioData::Unit>(data).size() == 1);
    REQUIRE(entriesOfType<ScenarioData::Structure>(data).size() == 2);
}

namespace {

std::vector<std::string> briefingFrom(const std::string& text, int scenario, eIniSection section)
{
    std::istringstream stream(text);
    return parseBriefing(stream, scenario, section);
}

const char* const BRIEFING_FILE =
    "[SCEN]\n"
    "Number=1\n"
    "[BRIEFING]\n"
    "Text=\"First briefing line\"\n"
    "Text=\"Second briefing line\"\n"
    "[WIN]\n"
    "Text=\"You won the first\"\n"
    "[LOSE]\n"
    "Text=\"You lost the first\"\n"
    "[SCEN]\n"
    "Number=2\n"
    "[BRIEFING]\n"
    "Text=\"Briefing of the second\"\n"
    "[WIN]\n"
    "Text=\"You won the second\"\n";

} // namespace

TEST_CASE("parseBriefing - reads the lines of the requested scenario and section", "[ini_briefing]")
{
    auto lines = briefingFrom(BRIEFING_FILE, 1, eIniSection::BRIEFING);
    REQUIRE(lines.size() == 2);
    REQUIRE(lines[0] == "First briefing line");
    REQUIRE(lines[1] == "Second briefing line");
}

TEST_CASE("parseBriefing - section selects win and lose texts", "[ini_briefing]")
{
    auto win = briefingFrom(BRIEFING_FILE, 1, eIniSection::WIN);
    REQUIRE(win.size() == 1);
    REQUIRE(win[0] == "You won the first");

    auto lose = briefingFrom(BRIEFING_FILE, 1, eIniSection::LOSE);
    REQUIRE(lose.size() == 1);
    REQUIRE(lose[0] == "You lost the first");
}

TEST_CASE("parseBriefing - other scenario numbers", "[ini_briefing]")
{
    auto briefing = briefingFrom(BRIEFING_FILE, 2, eIniSection::BRIEFING);
    REQUIRE(briefing.size() == 1);
    REQUIRE(briefing[0] == "Briefing of the second");

    auto win = briefingFrom(BRIEFING_FILE, 2, eIniSection::WIN);
    REQUIRE(win.size() == 1);
    REQUIRE(win[0] == "You won the second");
}

TEST_CASE("parseBriefing - unknown scenario or empty section gives nothing", "[ini_briefing]")
{
    REQUIRE(briefingFrom(BRIEFING_FILE, 3, eIniSection::BRIEFING).empty());
    REQUIRE(briefingFrom(BRIEFING_FILE, 2, eIniSection::LOSE).empty());
    REQUIRE(briefingFrom("", 1, eIniSection::BRIEFING).empty());
}

TEST_CASE("parseBriefing - CRLF, comments and unquoted text", "[ini_briefing]")
{
    auto lines = briefingFrom(
        "; comment\r\n"
        "[SCEN]\r\n"
        "Number=1\r\n"
        "[BRIEFING]\r\n"
        "Text=Plain text\r\n"
        "Text=\"Quoted text\"\r\n",
        1, eIniSection::BRIEFING);
    REQUIRE(lines.size() == 2);
    REQUIRE(lines[0] == "Plain text");
    REQUIRE(lines[1] == "Quoted text");
}

TEST_CASE("parseBriefing - description section is read like the others", "[ini_briefing]")
{
    auto lines = briefingFrom(
        "[SCEN]\n"
        "Number=0\n"
        "[DESCRIPTION]\n"
        "Text=\"House description\"\n",
        0, eIniSection::DESCRIPTION);
    REQUIRE(lines.size() == 1);
    REQUIRE(lines[0] == "House description");
}

TEST_CASE("parseBriefing - stops after ten sentences", "[ini_briefing]")
{
    std::string text = "[SCEN]\nNumber=1\n[BRIEFING]\n";
    for (int i = 0; i < 15; i++) {
        text += "Text=\"line " + std::to_string(i) + "\"\n";
    }
    auto lines = briefingFrom(text, 1, eIniSection::BRIEFING);
    REQUIRE(lines.size() == 10);
    REQUIRE(lines[0] == "line 0");
    REQUIRE(lines[9] == "line 9");
}

TEST_CASE("parseBriefing - non-text words are ignored", "[ini_briefing]")
{
    auto lines = briefingFrom(
        "[SCEN]\n"
        "Number=1\n"
        "[BRIEFING]\n"
        "Other=\"ignored\"\n"
        "Text=\"kept\"\n",
        1, eIniSection::BRIEFING);
    REQUIRE(lines.size() == 1);
    REQUIRE(lines[0] == "kept");
}

namespace {

RegionFileData regionsFrom(const std::string& text)
{
    std::istringstream stream(text);
    return parseRegionFile(stream);
}

} // namespace

TEST_CASE("parseRegionFile - empty input", "[ini_region]")
{
    auto data = regionsFrom("");
    REQUIRE(data.worldRegions.empty());
    REQUIRE(data.conquests.empty());
}

TEST_CASE("parseRegionFile - region house makes the region visible", "[ini_region]")
{
    auto data = regionsFrom(
        "Region=3\n"
        "House=Atreides\n");
    REQUIRE(data.worldRegions.size() == 1);
    const auto& region = data.worldRegions.at(2);
    REQUIRE(region.house == ATREIDES);
    REQUIRE(region.alpha == 255);
    REQUIRE_FALSE(region.selectable.has_value());
}

TEST_CASE("parseRegionFile - region select makes the region selectable with minimal alpha", "[ini_region]")
{
    auto data = regionsFrom(
        "Region=5\n"
        "Select=1\n");
    const auto& region = data.worldRegions.at(4);
    REQUIRE(region.selectable == true);
    REQUIRE(region.alpha == 1);
    REQUIRE_FALSE(region.house.has_value());
}

TEST_CASE("parseRegionFile - select can be false", "[ini_region]")
{
    auto data = regionsFrom(
        "Region=5\n"
        "Select=0\n");
    REQUIRE(data.worldRegions.at(4).selectable == false);
}

TEST_CASE("parseRegionFile - the last of House and Select decides the alpha", "[ini_region]")
{
    auto houseLast = regionsFrom(
        "Region=1\n"
        "Select=1\n"
        "House=Ordos\n");
    REQUIRE(houseLast.worldRegions.at(0).alpha == 255);

    auto selectLast = regionsFrom(
        "Region=1\n"
        "House=Ordos\n"
        "Select=1\n");
    REQUIRE(selectLast.worldRegions.at(0).alpha == 1);
    REQUIRE(selectLast.worldRegions.at(0).house == ORDOS);
}

TEST_CASE("parseRegionFile - several regions are tracked independently", "[ini_region]")
{
    auto data = regionsFrom(
        "Region=1\n"
        "House=Atreides\n"
        "Region=2\n"
        "House=Harkonnen\n"
        "Select=1\n");
    REQUIRE(data.worldRegions.size() == 2);
    REQUIRE(data.worldRegions.at(0).house == ATREIDES);
    REQUIRE_FALSE(data.worldRegions.at(0).selectable.has_value());
    REQUIRE(data.worldRegions.at(1).house == HARKONNEN);
    REQUIRE(data.worldRegions.at(1).selectable == true);
}

TEST_CASE("parseRegionFile - region settings before any region are ignored", "[ini_region]")
{
    auto data = regionsFrom(
        "House=Atreides\n"
        "Select=1\n"
        "Text=\"orphan\"\n");
    REQUIRE(data.worldRegions.empty());
    REQUIRE(data.conquests.empty());
}

TEST_CASE("parseRegionFile - conquer regions are numbered in file order", "[ini_region]")
{
    auto data = regionsFrom(
        "RegionConquer=4\n"
        "House=Atreides\n"
        "Text=\"First conquest\"\n"
        "RegionConquer=9\n"
        "House=Harkonnen\n"
        "Text=\"Second conquest\"\n");
    REQUIRE(data.conquests.size() == 2);

    REQUIRE(data.conquests[0].regionConquer == 3);
    REQUIRE(data.conquests[0].house == ATREIDES);
    REQUIRE(data.conquests[0].text == "First conquest");

    REQUIRE(data.conquests[1].regionConquer == 8);
    REQUIRE(data.conquests[1].house == HARKONNEN);
    REQUIRE(data.conquests[1].text == "Second conquest");

    REQUIRE(data.worldRegions.empty());
}

TEST_CASE("parseRegionFile - a conquer without house or text keeps them empty", "[ini_region]")
{
    auto data = regionsFrom("RegionConquer=2\n");
    REQUIRE(data.conquests.size() == 1);
    REQUIRE(data.conquests[0].regionConquer == 1);
    REQUIRE_FALSE(data.conquests[0].house.has_value());
    REQUIRE_FALSE(data.conquests[0].text.has_value());
}

TEST_CASE("parseRegionFile - a plain region after a conquer stops adding to the conquer", "[ini_region]")
{
    auto data = regionsFrom(
        "RegionConquer=1\n"
        "Region=7\n"
        "House=Ordos\n"
        "Text=\"not for the conquer\"\n");
    REQUIRE(data.conquests.size() == 1);
    REQUIRE_FALSE(data.conquests[0].house.has_value());
    REQUIRE_FALSE(data.conquests[0].text.has_value());
    REQUIRE(data.worldRegions.at(6).house == ORDOS);
}

TEST_CASE("parseRegionFile - a conquer after a region stops adding to the region", "[ini_region]")
{
    auto data = regionsFrom(
        "Region=7\n"
        "RegionConquer=2\n"
        "House=Ordos\n"
        "Select=1\n");
    REQUIRE(data.worldRegions.empty());
    REQUIRE(data.conquests.size() == 1);
    REQUIRE(data.conquests[0].house == ORDOS);
}

TEST_CASE("parseRegionFile - invalid lines, comments and CRLF are skipped", "[ini_region]")
{
    auto data = regionsFrom(
        "; comment\r\n"
        "no equals sign\r\n"
        "=novalue\r\n"
        "Key=\r\n"
        "Region=2\r\n"
        "House=Fremen\r\n");
    REQUIRE(data.worldRegions.size() == 1);
    REQUIRE(data.worldRegions.at(1).house == FREMEN);
}

TEST_CASE("parseRegionFile - region number zero or negative is not stored", "[ini_region]")
{
    auto data = regionsFrom(
        "Region=0\n"
        "House=Atreides\n");
    REQUIRE(data.worldRegions.empty());
}
