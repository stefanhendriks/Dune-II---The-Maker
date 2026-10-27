#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "utils/IniParsing.hpp"

using Catch::Matchers::WithinAbs;

TEST_CASE("ToInt - parses valid integers", "[ini_parsing]")
{
    REQUIRE(ToInt("0") == 0);
    REQUIRE(ToInt("42") == 42);
    REQUIRE(ToInt("-17") == -17);
}

TEST_CASE("ToInt - invalid input returns 0", "[ini_parsing]")
{
    REQUIRE(ToInt("") == 0);
    REQUIRE(ToInt("abc") == 0);
    REQUIRE(ToInt("12abc") == 0);
    REQUIRE(ToInt("99999999999999999999") == 0);
}

TEST_CASE("ToFloat - parses valid floats", "[ini_parsing]")
{
    REQUIRE_THAT(ToFloat("1.5"), WithinAbs(1.5f, 0.0001));
    REQUIRE_THAT(ToFloat("-0.25"), WithinAbs(-0.25f, 0.0001));
    REQUIRE_THAT(ToFloat("3"), WithinAbs(3.0f, 0.0001));
}

TEST_CASE("ToFloat - invalid input returns 0", "[ini_parsing]")
{
    REQUIRE_THAT(ToFloat(""), WithinAbs(0.0f, 0.0001));
    REQUIRE_THAT(ToFloat("abc"), WithinAbs(0.0f, 0.0001));
    REQUIRE_THAT(ToFloat("1.5x"), WithinAbs(0.0f, 0.0001));
}

TEST_CASE("ToBool - accepts 1, true and yes case-insensitively", "[ini_parsing]")
{
    REQUIRE(ToBool("1"));
    REQUIRE(ToBool("true"));
    REQUIRE(ToBool("TRUE"));
    REQUIRE(ToBool("True"));
    REQUIRE(ToBool("yes"));
    REQUIRE(ToBool("YES"));
}

TEST_CASE("ToBool - everything else is false", "[ini_parsing]")
{
    REQUIRE_FALSE(ToBool("0"));
    REQUIRE_FALSE(ToBool("false"));
    REQUIRE_FALSE(ToBool("no"));
    REQUIRE_FALSE(ToBool(""));
    REQUIRE_FALSE(ToBool("2"));
    REQUIRE_FALSE(ToBool(" true"));
}

TEST_CASE("ToTrim - removes leading and trailing whitespace", "[ini_parsing]")
{
    REQUIRE(ToTrim("  hello  ") == "hello");
    REQUIRE(ToTrim("\t hello\r\n") == "hello");
    REQUIRE(ToTrim("hello") == "hello");
    REQUIRE(ToTrim("a b") == "a b");
}

TEST_CASE("ToTrim - empty and whitespace-only strings give empty", "[ini_parsing]")
{
    REQUIRE(ToTrim("").empty());
    REQUIRE(ToTrim("   \t ").empty());
}

TEST_CASE("removeQuote - strips surrounding double quotes", "[ini_parsing]")
{
    REQUIRE(removeQuote("\"hello\"") == "hello");
    REQUIRE(removeQuote("hello") == "hello");
    REQUIRE(removeQuote("\"hello") == "hello");
    REQUIRE(removeQuote("hello\"") == "hello");
    REQUIRE(removeQuote("\"\"").empty());
    REQUIRE(removeQuote("").empty());
}

TEST_CASE("removeQuote - keeps inner quotes", "[ini_parsing]")
{
    REQUIRE(removeQuote("\"say \"hi\" now\"") == "say \"hi\" now");
}

TEST_CASE("splitByComma - splits into untrimmed tokens", "[ini_parsing]")
{
    auto parts = splitByComma("Atreides,Trike, 100 ,4");
    REQUIRE(parts.size() == 4);
    REQUIRE(parts[0] == "Atreides");
    REQUIRE(parts[1] == "Trike");
    REQUIRE(parts[2] == " 100 ");
    REQUIRE(parts[3] == "4");
}

TEST_CASE("splitByComma - single token and empty input", "[ini_parsing]")
{
    REQUIRE(splitByComma("one").size() == 1);
    REQUIRE(splitByComma("").empty());
}

TEST_CASE("splitByComma - consecutive commas give empty tokens, trailing comma does not", "[ini_parsing]")
{
    auto parts = splitByComma("a,,b,");
    REQUIRE(parts.size() == 3);
    REQUIRE(parts[0] == "a");
    REQUIRE(parts[1].empty());
    REQUIRE(parts[2] == "b");
}

TEST_CASE("extractTagValue - returns the trimmed value after the prefix", "[ini_parsing]")
{
    REQUIRE(extractTagValue("[UNIT: Devastator]", "[UNIT:") == "Devastator");
    REQUIRE(extractTagValue("[UNIT:Trike]", "[UNIT:") == "Trike");
    REQUIRE(extractTagValue("[STRUCTURE:  Refinery ]", "[STRUCTURE:") == "Refinery");
}

TEST_CASE("extractTagValue - missing prefix gives empty string", "[ini_parsing]")
{
    REQUIRE(extractTagValue("[BULLET: Rocket]", "[UNIT:").empty());
    REQUIRE(extractTagValue("", "[UNIT:").empty());
}

TEST_CASE("extractTagValue - missing closing bracket takes the rest of the line", "[ini_parsing]")
{
    REQUIRE(extractTagValue("[UNIT: Trike", "[UNIT:") == "Trike");
}

TEST_CASE("extractSectionName - returns the trimmed section name", "[ini_parsing]")
{
    REQUIRE(extractSectionName("[UNITS]") == "UNITS");
    REQUIRE(extractSectionName("[ Atreides ]") == "Atreides");
    REQUIRE(extractSectionName("  [MAP]  ") == "MAP");
}

TEST_CASE("extractSectionName - invalid section lines give empty string", "[ini_parsing]")
{
    REQUIRE(extractSectionName("").empty());
    REQUIRE(extractSectionName("no brackets").empty());
    REQUIRE(extractSectionName("[]").empty());
    REQUIRE(extractSectionName("[   ]").empty());
    REQUIRE(extractSectionName("[unterminated").empty());
    REQUIRE(extractSectionName("]reversed[").empty());
}

TEST_CASE("INI_SplitWord - splits on the first equals sign", "[ini_parsing]")
{
    auto [key, value] = INI_SplitWord("Credits=2000");
    REQUIRE(key == "Credits");
    REQUIRE(value == "2000");
}

TEST_CASE("INI_SplitWord - keeps further equals signs in the value", "[ini_parsing]")
{
    auto [key, value] = INI_SplitWord("Text=a=b=c");
    REQUIRE(key == "Text");
    REQUIRE(value == "a=b=c");
}

TEST_CASE("INI_SplitWord - no equals sign gives the whole input and an empty value", "[ini_parsing]")
{
    auto [key, value] = INI_SplitWord("Credits");
    REQUIRE(key == "Credits");
    REQUIRE(value.empty());
}

TEST_CASE("INI_SplitWord - empty key and empty value", "[ini_parsing]")
{
    auto [key1, value1] = INI_SplitWord("=5");
    REQUIRE(key1.empty());
    REQUIRE(value1 == "5");

    auto [key2, value2] = INI_SplitWord("Key=");
    REQUIRE(key2 == "Key");
    REQUIRE(value2.empty());
}

TEST_CASE("isCommentLine - recognises comment and empty lines", "[ini_parsing]")
{
    REQUIRE(isCommentLine(""));
    REQUIRE(isCommentLine("; comment"));
    REQUIRE(isCommentLine("# comment"));
    REQUIRE(isCommentLine("// comment"));
    REQUIRE(isCommentLine("\n"));
}

TEST_CASE("isCommentLine - regular lines are not comments", "[ini_parsing]")
{
    REQUIRE_FALSE(isCommentLine("[UNITS]"));
    REQUIRE_FALSE(isCommentLine("Credits=2000"));
    REQUIRE_FALSE(isCommentLine("/ single slash"));
    REQUIRE_FALSE(isCommentLine("/"));
    REQUIRE_FALSE(isCommentLine(" ; leading space"));
}

TEST_CASE("SCEN_INI_SectionType - maps scenario section names", "[ini_parsing][sections]")
{
    REQUIRE(SCEN_INI_SectionType("UNITS") == eScenarioIniSection::UNITS);
    REQUIRE(SCEN_INI_SectionType("SKIRMISH") == eScenarioIniSection::SKIRMISH);
    REQUIRE(SCEN_INI_SectionType("STRUCTURES") == eScenarioIniSection::STRUCTURES);
    REQUIRE(SCEN_INI_SectionType("REINFORCEMENTS") == eScenarioIniSection::REINFORCEMENTS);
    REQUIRE(SCEN_INI_SectionType("MAP") == eScenarioIniSection::MAP);
    REQUIRE(SCEN_INI_SectionType("BASIC") == eScenarioIniSection::BASIC);
}

TEST_CASE("SCEN_INI_SectionType - maps house sections and is case-insensitive", "[ini_parsing][sections]")
{
    REQUIRE(SCEN_INI_SectionType("Atreides") == eScenarioIniSection::HOUSEATREIDES);
    REQUIRE(SCEN_INI_SectionType("ORDOS") == eScenarioIniSection::HOUSEORDOS);
    REQUIRE(SCEN_INI_SectionType("harkonnen") == eScenarioIniSection::HOUSEHARKONNEN);
    REQUIRE(SCEN_INI_SectionType("Sardaukar") == eScenarioIniSection::HOUSESARDAUKAR);
    REQUIRE(SCEN_INI_SectionType("Fremen") == eScenarioIniSection::HOUSEFREMEN);
    REQUIRE(SCEN_INI_SectionType("Mercenary") == eScenarioIniSection::HOUSEMERCENARY);
    REQUIRE(SCEN_INI_SectionType("units") == eScenarioIniSection::UNITS);
}

TEST_CASE("SCEN_INI_SectionType - every house section is a house section", "[ini_parsing][sections]")
{
    for (const char* name : {"Atreides", "Ordos", "Harkonnen", "Sardaukar", "Fremen", "Mercenary"}) {
        REQUIRE(isHouseSection(SCEN_INI_SectionType(name)));
    }
    REQUIRE_FALSE(isHouseSection(SCEN_INI_SectionType("SKIRMISH")));
    REQUIRE_FALSE(isHouseSection(SCEN_INI_SectionType("BASIC")));
}

TEST_CASE("SCEN_INI_SectionType - unknown section gives NONE", "[ini_parsing][sections]")
{
    REQUIRE(SCEN_INI_SectionType("DOES_NOT_EXIST") == eScenarioIniSection::NONE);
    REQUIRE(SCEN_INI_SectionType("") == eScenarioIniSection::NONE);
    REQUIRE(SCEN_INI_SectionType("BULLETS") == eScenarioIniSection::NONE);
}

TEST_CASE("SKIRMISH is not treated as a house section", "[ini_parsing][sections]")
{
    REQUIRE(SCEN_INI_SectionType("SKIRMISH") != SCEN_INI_SectionType("Atreides"));
    REQUIRE_FALSE(isHouseSection(eScenarioIniSection::SKIRMISH));
}

TEST_CASE("isHouseSection - bounds of the house range", "[ini_parsing][sections]")
{
    REQUIRE(isHouseSection(eScenarioIniSection::HOUSEATREIDES));
    REQUIRE(isHouseSection(eScenarioIniSection::HOUSEMERCENARY));
    REQUIRE_FALSE(isHouseSection(eScenarioIniSection::NONE));
    REQUIRE_FALSE(isHouseSection(eScenarioIniSection::BASIC));
    REQUIRE_FALSE(isHouseSection(eScenarioIniSection::REINFORCEMENTS));
}

TEST_CASE("GAME_INI_SectionType - maps game.ini sections", "[ini_parsing][sections]")
{
    REQUIRE(GAME_INI_SectionType("BULLETS", eGameIniSection::GAME) == eGameIniSection::BULLETS);
    REQUIRE(GAME_INI_SectionType("UNITS", eGameIniSection::GAME) == eGameIniSection::UNITS);
    REQUIRE(GAME_INI_SectionType("STRUCTURES", eGameIniSection::GAME) == eGameIniSection::STRUCTURES);
    REQUIRE(GAME_INI_SectionType("TERRAINS", eGameIniSection::GAME) == eGameIniSection::TERRAINS);
    REQUIRE(GAME_INI_SectionType("terrains", eGameIniSection::GAME) == eGameIniSection::TERRAINS);
}

TEST_CASE("GAME_INI_SectionType - unknown names keep the last known section", "[ini_parsing][sections]")
{
    REQUIRE(GAME_INI_SectionType("UNIT: Trike", eGameIniSection::UNITS) == eGameIniSection::UNITS);
    REQUIRE(GAME_INI_SectionType("STRUCTURE: Refinery", eGameIniSection::STRUCTURES) == eGameIniSection::STRUCTURES);
    REQUIRE(GAME_INI_SectionType("whatever", eGameIniSection::GAME) == eGameIniSection::GAME);
}

TEST_CASE("INI_WordType - REGION section", "[ini_parsing][words]")
{
    REQUIRE(ScenarioIniWordType("Region", eScenarioIniSection::REGION) == eIniWord::REGION);
    REQUIRE(ScenarioIniWordType("RegionConquer", eScenarioIniSection::REGION) == eIniWord::REGIONCONQUER);
    REQUIRE(ScenarioIniWordType("House", eScenarioIniSection::REGION) == eIniWord::REGIONHOUSE);
    REQUIRE(ScenarioIniWordType("Text", eScenarioIniSection::REGION) == eIniWord::REGIONTEXT);
    REQUIRE(ScenarioIniWordType("Select", eScenarioIniSection::REGION) == eIniWord::REGIONSELECT);
    REQUIRE(ScenarioIniWordType("Unknown", eScenarioIniSection::REGION) == eIniWord::NONE);
}

TEST_CASE("INI_WordType - briefing-like sections only know Number and Text", "[ini_parsing][words]")
{
    for (auto section : {eScenarioIniSection::SCEN, eScenarioIniSection::DESCRIPTION, eScenarioIniSection::BRIEFING,
                         eScenarioIniSection::ADVICE, eScenarioIniSection::LOSE, eScenarioIniSection::WIN}) {
        REQUIRE(ScenarioIniWordType("Number", section) == eIniWord::NUMBER);
        REQUIRE(ScenarioIniWordType("Text", section) == eIniWord::REGIONTEXT);
        REQUIRE(ScenarioIniWordType("Credits", section) == eIniWord::NONE);
    }
}

TEST_CASE("INI_WordType - SKIRMISH section", "[ini_parsing][words]")
{
    REQUIRE(ScenarioIniWordType("Title", eScenarioIniSection::SKIRMISH) == eIniWord::MAPNAME);
    REQUIRE(ScenarioIniWordType("StartCell", eScenarioIniSection::SKIRMISH) == eIniWord::STARTCELL);
    REQUIRE(ScenarioIniWordType("Credits", eScenarioIniSection::SKIRMISH) == eIniWord::NONE);
}

TEST_CASE("INI_WordType - GAME and BASIC sections", "[ini_parsing][words]")
{
    REQUIRE(GameIniWordType("ModId", eGameIniSection::GAME) == eIniWord::MODID);
    REQUIRE(ScenarioIniWordType("CursorPos", eScenarioIniSection::BASIC) == eIniWord::FOCUS_POS);
    REQUIRE(ScenarioIniWordType("BriefPicture", eScenarioIniSection::BASIC) == eIniWord::BRIEFPICTURE);
    REQUIRE(ScenarioIniWordType("WinFlags", eScenarioIniSection::BASIC) == eIniWord::WINFLAGS);
    REQUIRE(ScenarioIniWordType("LoseFlags", eScenarioIniSection::BASIC) == eIniWord::LOSEFLAGS);
    REQUIRE(ScenarioIniWordType("ModId", eScenarioIniSection::BASIC) == eIniWord::NONE);
}

TEST_CASE("INI_WordType - every house section understands house words", "[ini_parsing][words]")
{
    for (auto section : {eScenarioIniSection::HOUSEATREIDES, eScenarioIniSection::HOUSEORDOS, eScenarioIniSection::HOUSEHARKONNEN,
                         eScenarioIniSection::HOUSESARDAUKAR, eScenarioIniSection::HOUSEFREMEN, eScenarioIniSection::HOUSEMERCENARY}) {
        REQUIRE(ScenarioIniWordType("Team", section) == eIniWord::TEAM);
        REQUIRE(ScenarioIniWordType("Credits", section) == eIniWord::CREDITS);
        REQUIRE(ScenarioIniWordType("Quota", section) == eIniWord::QUOTA);
        REQUIRE(ScenarioIniWordType("Brain", section) == eIniWord::BRAIN);
        REQUIRE(ScenarioIniWordType("House", section) == eIniWord::HOUSE);
        REQUIRE(ScenarioIniWordType("Focus", section) == eIniWord::FOCUS_POS);
    }
}

TEST_CASE("INI_WordType - MAP section", "[ini_parsing][words]")
{
    REQUIRE(ScenarioIniWordType("Width", eScenarioIniSection::MAP) == eIniWord::MAPWIDTH);
    REQUIRE(ScenarioIniWordType("Height", eScenarioIniSection::MAP) == eIniWord::MAPHEIGHT);
    REQUIRE(ScenarioIniWordType("Seed", eScenarioIniSection::MAP) == eIniWord::MAPSEED);
    REQUIRE(ScenarioIniWordType("Bloom", eScenarioIniSection::MAP) == eIniWord::MAPBLOOM);
    REQUIRE(ScenarioIniWordType("Field", eScenarioIniSection::MAP) == eIniWord::MAPFIELD);
}

TEST_CASE("INI_WordType - BULLETS section", "[ini_parsing][words]")
{
    REQUIRE(GameIniWordType("Bitmap", eGameIniSection::BULLETS) == eIniWord::BITMAP);
    REQUIRE(GameIniWordType("BitmapExplosion", eGameIniSection::BULLETS) == eIniWord::BITMAP_DEAD);
    REQUIRE(GameIniWordType("BitmapWidth", eGameIniSection::BULLETS) == eIniWord::BITMAP_WIDTH);
    REQUIRE(GameIniWordType("BitmapFrames", eGameIniSection::BULLETS) == eIniWord::BITMAP_FRAMES);
    REQUIRE(GameIniWordType("BitmapExplFrames", eGameIniSection::BULLETS) == eIniWord::BITMAP_DEADFRAMES);
    REQUIRE(GameIniWordType("DamageVehicle", eGameIniSection::BULLETS) == eIniWord::DAMAGE_VEHICLE);
    REQUIRE(GameIniWordType("DamageInfantry", eGameIniSection::BULLETS) == eIniWord::DAMAGE_INFANTRY);
    REQUIRE(GameIniWordType("Definition", eGameIniSection::BULLETS) == eIniWord::DEFINITION);
    REQUIRE(GameIniWordType("Sound", eGameIniSection::BULLETS) == eIniWord::SOUND);
    REQUIRE(GameIniWordType("ExplosionSize", eGameIniSection::BULLETS) == eIniWord::EXPLOSION_SIZE);
    REQUIRE(GameIniWordType("DeviateProbability", eGameIniSection::BULLETS) == eIniWord::DEVIATE_PROBABILITY);
    REQUIRE(GameIniWordType("GroundBullet", eGameIniSection::BULLETS) == eIniWord::GROUND_BULLET);
}

TEST_CASE("INI_WordType - STRUCTURES section", "[ini_parsing][words]")
{
    REQUIRE(GameIniWordType("HitPoints", eGameIniSection::STRUCTURES) == eIniWord::HITPOINTS);
    REQUIRE(GameIniWordType("FixPoints", eGameIniSection::STRUCTURES) == eIniWord::FIXHP);
    REQUIRE(GameIniWordType("PowerDrain", eGameIniSection::STRUCTURES) == eIniWord::POWERDRAIN);
    REQUIRE(GameIniWordType("HasConcrete", eGameIniSection::STRUCTURES) == eIniWord::HAS_CONCRETE);
    REQUIRE(GameIniWordType("PowerGive", eGameIniSection::STRUCTURES) == eIniWord::POWERGIVE);
    REQUIRE(GameIniWordType("Cost", eGameIniSection::STRUCTURES) == eIniWord::COST);
    REQUIRE(GameIniWordType("BuildTime", eGameIniSection::STRUCTURES) == eIniWord::BUILDTIME);
    REQUIRE(GameIniWordType("UponDestructionSpawnUnitAmountMin", eGameIniSection::STRUCTURES) == eIniWord::UPON_DESTRUCTION_SPAWN_UNIT_AMOUNT_MIN);
    REQUIRE(GameIniWordType("UponDestructionSpawnUnitAmountMax", eGameIniSection::STRUCTURES) == eIniWord::UPON_DESTRUCTION_SPAWN_UNIT_AMOUNT_MAX);
    REQUIRE(GameIniWordType("UponDestructionSpawnUnitType", eGameIniSection::STRUCTURES) == eIniWord::UPON_DESTRUCTION_SPAWN_UNIT_TYPE);
}

TEST_CASE("INI_WordType - UNITS section numeric and boolean words", "[ini_parsing][words]")
{
    REQUIRE(GameIniWordType("Bitmap", eGameIniSection::UNITS) == eIniWord::BITMAP);
    REQUIRE(GameIniWordType("BitmapTop", eGameIniSection::UNITS) == eIniWord::BITMAP_TOP);
    REQUIRE(GameIniWordType("Icon", eGameIniSection::UNITS) == eIniWord::ICON);
    REQUIRE(GameIniWordType("HitPoints", eGameIniSection::UNITS) == eIniWord::HITPOINTS);
    REQUIRE(GameIniWordType("Appetite", eGameIniSection::UNITS) == eIniWord::APPETITE);
    REQUIRE(GameIniWordType("MoveSpeed", eGameIniSection::UNITS) == eIniWord::MOVESPEED);
    REQUIRE(GameIniWordType("TurnSpeed", eGameIniSection::UNITS) == eIniWord::TURNSPEED);
    REQUIRE(GameIniWordType("AttackFrequency", eGameIniSection::UNITS) == eIniWord::ATTACKFREQ);
    REQUIRE(GameIniWordType("NextAttackFrequency", eGameIniSection::UNITS) == eIniWord::NEXTATTACKFREQ);
    REQUIRE(GameIniWordType("IsHarvester", eGameIniSection::UNITS) == eIniWord::ISHARVESTER);
    REQUIRE(GameIniWordType("FireTwice", eGameIniSection::UNITS) == eIniWord::FIRETWICE);
    REQUIRE(GameIniWordType("IsInfantry", eGameIniSection::UNITS) == eIniWord::ISINFANTRY);
    REQUIRE(GameIniWordType("Squishable", eGameIniSection::UNITS) == eIniWord::ISSQUISHABLE);
    REQUIRE(GameIniWordType("CanSquish", eGameIniSection::UNITS) == eIniWord::CANSQUISH);
    REQUIRE(GameIniWordType("IsAirborn", eGameIniSection::UNITS) == eIniWord::ISAIRBORN);
    REQUIRE(GameIniWordType("AbleToCarry", eGameIniSection::UNITS) == eIniWord::ABLETOCARRY);
    REQUIRE(GameIniWordType("FreeRoam", eGameIniSection::UNITS) == eIniWord::FREEROAM);
    REQUIRE(GameIniWordType("MaxCredits", eGameIniSection::UNITS) == eIniWord::HARVESTLIMIT);
    REQUIRE(GameIniWordType("HarvestSpeed", eGameIniSection::UNITS) == eIniWord::HARVESTSPEED);
    REQUIRE(GameIniWordType("HarvestAmount", eGameIniSection::UNITS) == eIniWord::HARVESTAMOUNT);
    REQUIRE(GameIniWordType("CanAttackAirUnits", eGameIniSection::UNITS) == eIniWord::CANATTACKAIRUNITS);
    REQUIRE(GameIniWordType("CanAttackUnits", eGameIniSection::UNITS) == eIniWord::CANATTACKUNITS);
    REQUIRE(GameIniWordType("AttackIsEnterStructure", eGameIniSection::UNITS) == eIniWord::ATTACK_IS_ENTER_STRUCTURE);
    REQUIRE(GameIniWordType("DamageOnEnterStructure", eGameIniSection::UNITS) == eIniWord::DAMAGE_ON_ENTER_STRUCTURE);
}

TEST_CASE("INI_WordType - HOUSES section", "[ini_parsing][words]")
{
    REQUIRE(GameIniWordType("ColorR", eGameIniSection::HOUSES) == eIniWord::RED);
    REQUIRE(GameIniWordType("ColorG", eGameIniSection::HOUSES) == eIniWord::GREEN);
    REQUIRE(GameIniWordType("ColorB", eGameIniSection::HOUSES) == eIniWord::BLUE);
    REQUIRE(GameIniWordType("FirePower", eGameIniSection::HOUSES) == eIniWord::FIREPOWER);
    REQUIRE(GameIniWordType("FireRate", eGameIniSection::HOUSES) == eIniWord::FIRERATE);
    REQUIRE(GameIniWordType("StructPrice", eGameIniSection::HOUSES) == eIniWord::STRUCTPRICE);
    REQUIRE(GameIniWordType("UnitPrice", eGameIniSection::HOUSES) == eIniWord::UNITPRICE);
    REQUIRE(GameIniWordType("Speed", eGameIniSection::HOUSES) == eIniWord::SPEED);
    REQUIRE(GameIniWordType("BuildSpeed", eGameIniSection::HOUSES) == eIniWord::BUILDSPEED);
    REQUIRE(GameIniWordType("HarvestSpeed", eGameIniSection::HOUSES) == eIniWord::HARVESTSPEED);
    REQUIRE(GameIniWordType("DumpSpeed", eGameIniSection::HOUSES) == eIniWord::DUMPSPEED);
}

TEST_CASE("INI_WordType - matching is case-insensitive", "[ini_parsing][words]")
{
    REQUIRE(ScenarioIniWordType("credits", eScenarioIniSection::HOUSEATREIDES) == eIniWord::CREDITS);
    REQUIRE(ScenarioIniWordType("CREDITS", eScenarioIniSection::HOUSEATREIDES) == eIniWord::CREDITS);
    REQUIRE(GameIniWordType("hitpoints", eGameIniSection::UNITS) == eIniWord::HITPOINTS);
}

TEST_CASE("INI_WordType - words from another section are not recognised", "[ini_parsing][words]")
{
    REQUIRE(ScenarioIniWordType("Credits", eScenarioIniSection::MAP) == eIniWord::NONE);
    REQUIRE(ScenarioIniWordType("Width", eScenarioIniSection::HOUSEORDOS) == eIniWord::NONE);
    REQUIRE(GameIniWordType("MoveSpeed", eGameIniSection::STRUCTURES) == eIniWord::NONE);
    REQUIRE(ScenarioIniWordType("Seed", eScenarioIniSection::NONE) == eIniWord::NONE);
}

TEST_CASE("INI_WordType - empty word gives NONE", "[ini_parsing][words]")
{
    REQUIRE(GameIniWordType("", eGameIniSection::UNITS) == eIniWord::NONE);
    REQUIRE(ScenarioIniWordType("", eScenarioIniSection::MAP) == eIniWord::NONE);
}

TEST_CASE("Scenario line workflow - section header then key=value", "[ini_parsing][workflow]")
{
    auto sectionName = extractSectionName("[Atreides]");
    auto section = SCEN_INI_SectionType(sectionName);
    REQUIRE(section == eScenarioIniSection::HOUSEATREIDES);

    auto [left, right] = INI_SplitWord("Credits=2500");
    REQUIRE(ScenarioIniWordType(left, section) == eIniWord::CREDITS);
    REQUIRE(ToInt(right) == 2500);
}

TEST_CASE("Scenario line workflow - unit line fields", "[ini_parsing][workflow]")
{
    auto [key, value] = INI_SplitWord("ID001=Atreides, Trike, 256, 1234, 64, 64");
    REQUIRE(key == "ID001");

    auto parts = splitByComma(value);
    REQUIRE(parts.size() == 6);
    REQUIRE(ToTrim(parts[0]) == "Atreides");
    REQUIRE(ToTrim(parts[1]) == "Trike");
    REQUIRE(ToInt(ToTrim(parts[3])) == 1234);
}

TEST_CASE("Briefing workflow - quoted text lines", "[ini_parsing][workflow]")
{
    auto [left, right] = INI_SplitWord("Text=\"Welcome to Arrakis\"");
    REQUIRE(ScenarioIniWordType(left, eScenarioIniSection::BRIEFING) == eIniWord::REGIONTEXT);
    REQUIRE(removeQuote(right) == "Welcome to Arrakis");
}

TEST_CASE("Briefing workflow - scenario number line", "[ini_parsing][workflow]")
{
    auto [left, right] = INI_SplitWord("Number=3");
    REQUIRE(ScenarioIniWordType(left, eScenarioIniSection::SCEN) == eIniWord::NUMBER);
    REQUIRE(ToInt(right) == 3);
}

TEST_CASE("GameIniWordType - NONE section knows no words", "[ini_parsing][words]")
{
    REQUIRE(GameIniWordType("ModId", eGameIniSection::NONE) == eIniWord::NONE);
    REQUIRE(GameIniWordType("Bitmap", eGameIniSection::NONE) == eIniWord::NONE);
}

TEST_CASE("ScenarioIniWordType - game.ini words are not recognised in scenario sections", "[ini_parsing][words]")
{
    REQUIRE(ScenarioIniWordType("ModId", eScenarioIniSection::NONE) == eIniWord::NONE);
    REQUIRE(ScenarioIniWordType("HitPoints", eScenarioIniSection::UNITS) == eIniWord::NONE);
    REQUIRE(ScenarioIniWordType("Cost", eScenarioIniSection::STRUCTURES) == eIniWord::NONE);
}
