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
    REQUIRE(SCEN_INI_SectionType("UNITS") == eIniSection::UNITS);
    REQUIRE(SCEN_INI_SectionType("SKIRMISH") == eIniSection::SKIRMISH);
    REQUIRE(SCEN_INI_SectionType("STRUCTURES") == eIniSection::STRUCTURES);
    REQUIRE(SCEN_INI_SectionType("REINFORCEMENTS") == eIniSection::REINFORCEMENTS);
    REQUIRE(SCEN_INI_SectionType("MAP") == eIniSection::MAP);
    REQUIRE(SCEN_INI_SectionType("BASIC") == eIniSection::BASIC);
}

TEST_CASE("SCEN_INI_SectionType - maps house sections and is case-insensitive", "[ini_parsing][sections]")
{
    REQUIRE(SCEN_INI_SectionType("Atreides") == eIniSection::HOUSEATREIDES);
    REQUIRE(SCEN_INI_SectionType("ORDOS") == eIniSection::HOUSEORDOS);
    REQUIRE(SCEN_INI_SectionType("harkonnen") == eIniSection::HOUSEHARKONNEN);
    REQUIRE(SCEN_INI_SectionType("Sardaukar") == eIniSection::HOUSESARDAUKAR);
    REQUIRE(SCEN_INI_SectionType("Fremen") == eIniSection::HOUSEFREMEN);
    REQUIRE(SCEN_INI_SectionType("Mercenary") == eIniSection::HOUSEMERCENARY);
    REQUIRE(SCEN_INI_SectionType("units") == eIniSection::UNITS);
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
    REQUIRE(SCEN_INI_SectionType("DOES_NOT_EXIST") == eIniSection::NONE);
    REQUIRE(SCEN_INI_SectionType("") == eIniSection::NONE);
    REQUIRE(SCEN_INI_SectionType("BULLETS") == eIniSection::NONE);
}

TEST_CASE("SKIRMISH is not treated as a house section", "[ini_parsing][sections]")
{
    REQUIRE(SCEN_INI_SectionType("SKIRMISH") != SCEN_INI_SectionType("Atreides"));
    REQUIRE_FALSE(isHouseSection(eIniSection::SKIRMISH));
}

TEST_CASE("isHouseSection - bounds of the house range", "[ini_parsing][sections]")
{
    REQUIRE(isHouseSection(eIniSection::HOUSEATREIDES));
    REQUIRE(isHouseSection(eIniSection::HOUSEMERCENARY));
    REQUIRE_FALSE(isHouseSection(eIniSection::NONE));
    REQUIRE_FALSE(isHouseSection(eIniSection::BASIC));
    REQUIRE_FALSE(isHouseSection(eIniSection::REINFORCEMENTS));
    REQUIRE_FALSE(isHouseSection(eIniSection::HOUSES));
}

TEST_CASE("GAME_INI_SectionType - maps game.ini sections", "[ini_parsing][sections]")
{
    REQUIRE(GAME_INI_SectionType("BULLETS", eIniSection::GAME) == eIniSection::BULLETS);
    REQUIRE(GAME_INI_SectionType("UNITS", eIniSection::GAME) == eIniSection::UNITS);
    REQUIRE(GAME_INI_SectionType("STRUCTURES", eIniSection::GAME) == eIniSection::STRUCTURES);
    REQUIRE(GAME_INI_SectionType("TERRAINS", eIniSection::GAME) == eIniSection::TERRAINS);
    REQUIRE(GAME_INI_SectionType("terrains", eIniSection::GAME) == eIniSection::TERRAINS);
}

TEST_CASE("GAME_INI_SectionType - unknown names keep the last known section", "[ini_parsing][sections]")
{
    REQUIRE(GAME_INI_SectionType("UNIT: Trike", eIniSection::UNITS) == eIniSection::UNITS);
    REQUIRE(GAME_INI_SectionType("STRUCTURE: Refinery", eIniSection::STRUCTURES) == eIniSection::STRUCTURES);
    REQUIRE(GAME_INI_SectionType("whatever", eIniSection::GAME) == eIniSection::GAME);
}

TEST_CASE("INI_WordType - REGION section", "[ini_parsing][words]")
{
    REQUIRE(INI_WordType("Region", eIniSection::REGION) == eIniWord::REGION);
    REQUIRE(INI_WordType("RegionConquer", eIniSection::REGION) == eIniWord::REGIONCONQUER);
    REQUIRE(INI_WordType("House", eIniSection::REGION) == eIniWord::REGIONHOUSE);
    REQUIRE(INI_WordType("Text", eIniSection::REGION) == eIniWord::REGIONTEXT);
    REQUIRE(INI_WordType("Select", eIniSection::REGION) == eIniWord::REGIONSELECT);
    REQUIRE(INI_WordType("Unknown", eIniSection::REGION) == eIniWord::NONE);
}

TEST_CASE("INI_WordType - briefing-like sections only know Number and Text", "[ini_parsing][words]")
{
    for (auto section : {eIniSection::SCEN, eIniSection::DESCRIPTION, eIniSection::BRIEFING,
                         eIniSection::ADVICE, eIniSection::LOSE, eIniSection::WIN}) {
        REQUIRE(INI_WordType("Number", section) == eIniWord::NUMBER);
        REQUIRE(INI_WordType("Text", section) == eIniWord::REGIONTEXT);
        REQUIRE(INI_WordType("Credits", section) == eIniWord::NONE);
    }
}

TEST_CASE("INI_WordType - SKIRMISH section", "[ini_parsing][words]")
{
    REQUIRE(INI_WordType("Title", eIniSection::SKIRMISH) == eIniWord::MAPNAME);
    REQUIRE(INI_WordType("StartCell", eIniSection::SKIRMISH) == eIniWord::STARTCELL);
    REQUIRE(INI_WordType("Credits", eIniSection::SKIRMISH) == eIniWord::NONE);
}

TEST_CASE("INI_WordType - GAME and BASIC sections", "[ini_parsing][words]")
{
    REQUIRE(INI_WordType("ModId", eIniSection::GAME) == eIniWord::MODID);
    REQUIRE(INI_WordType("CursorPos", eIniSection::BASIC) == eIniWord::FOCUS_POS);
    REQUIRE(INI_WordType("BriefPicture", eIniSection::BASIC) == eIniWord::BRIEFPICTURE);
    REQUIRE(INI_WordType("WinFlags", eIniSection::BASIC) == eIniWord::WINFLAGS);
    REQUIRE(INI_WordType("LoseFlags", eIniSection::BASIC) == eIniWord::LOSEFLAGS);
    REQUIRE(INI_WordType("ModId", eIniSection::BASIC) == eIniWord::NONE);
}

TEST_CASE("INI_WordType - every house section understands house words", "[ini_parsing][words]")
{
    for (auto section : {eIniSection::HOUSEATREIDES, eIniSection::HOUSEORDOS, eIniSection::HOUSEHARKONNEN,
                         eIniSection::HOUSESARDAUKAR, eIniSection::HOUSEFREMEN, eIniSection::HOUSEMERCENARY}) {
        REQUIRE(INI_WordType("Team", section) == eIniWord::TEAM);
        REQUIRE(INI_WordType("Credits", section) == eIniWord::CREDITS);
        REQUIRE(INI_WordType("Quota", section) == eIniWord::QUOTA);
        REQUIRE(INI_WordType("Brain", section) == eIniWord::BRAIN);
        REQUIRE(INI_WordType("House", section) == eIniWord::HOUSE);
        REQUIRE(INI_WordType("Focus", section) == eIniWord::FOCUS_POS);
    }
}

TEST_CASE("INI_WordType - MAP section", "[ini_parsing][words]")
{
    REQUIRE(INI_WordType("Width", eIniSection::MAP) == eIniWord::MAPWIDTH);
    REQUIRE(INI_WordType("Height", eIniSection::MAP) == eIniWord::MAPHEIGHT);
    REQUIRE(INI_WordType("Seed", eIniSection::MAP) == eIniWord::MAPSEED);
    REQUIRE(INI_WordType("Bloom", eIniSection::MAP) == eIniWord::MAPBLOOM);
    REQUIRE(INI_WordType("Field", eIniSection::MAP) == eIniWord::MAPFIELD);
}

TEST_CASE("INI_WordType - BULLETS section", "[ini_parsing][words]")
{
    REQUIRE(INI_WordType("Bitmap", eIniSection::BULLETS) == eIniWord::BITMAP);
    REQUIRE(INI_WordType("BitmapExplosion", eIniSection::BULLETS) == eIniWord::BITMAP_DEAD);
    REQUIRE(INI_WordType("BitmapWidth", eIniSection::BULLETS) == eIniWord::BITMAP_WIDTH);
    REQUIRE(INI_WordType("BitmapFrames", eIniSection::BULLETS) == eIniWord::BITMAP_FRAMES);
    REQUIRE(INI_WordType("BitmapExplFrames", eIniSection::BULLETS) == eIniWord::BITMAP_DEADFRAMES);
    REQUIRE(INI_WordType("DamageVehicle", eIniSection::BULLETS) == eIniWord::DAMAGE_VEHICLE);
    REQUIRE(INI_WordType("DamageInfantry", eIniSection::BULLETS) == eIniWord::DAMAGE_INFANTRY);
    REQUIRE(INI_WordType("Definition", eIniSection::BULLETS) == eIniWord::DEFINITION);
    REQUIRE(INI_WordType("Sound", eIniSection::BULLETS) == eIniWord::SOUND);
    REQUIRE(INI_WordType("ExplosionSize", eIniSection::BULLETS) == eIniWord::EXPLOSION_SIZE);
    REQUIRE(INI_WordType("DeviateProbability", eIniSection::BULLETS) == eIniWord::DEVIATE_PROBABILITY);
    REQUIRE(INI_WordType("GroundBullet", eIniSection::BULLETS) == eIniWord::GROUND_BULLET);
}

TEST_CASE("INI_WordType - STRUCTURES section", "[ini_parsing][words]")
{
    REQUIRE(INI_WordType("HitPoints", eIniSection::STRUCTURES) == eIniWord::HITPOINTS);
    REQUIRE(INI_WordType("FixPoints", eIniSection::STRUCTURES) == eIniWord::FIXHP);
    REQUIRE(INI_WordType("PowerDrain", eIniSection::STRUCTURES) == eIniWord::POWERDRAIN);
    REQUIRE(INI_WordType("HasConcrete", eIniSection::STRUCTURES) == eIniWord::HAS_CONCRETE);
    REQUIRE(INI_WordType("PowerGive", eIniSection::STRUCTURES) == eIniWord::POWERGIVE);
    REQUIRE(INI_WordType("Cost", eIniSection::STRUCTURES) == eIniWord::COST);
    REQUIRE(INI_WordType("BuildTime", eIniSection::STRUCTURES) == eIniWord::BUILDTIME);
    REQUIRE(INI_WordType("UponDestructionSpawnUnitAmountMin", eIniSection::STRUCTURES) == eIniWord::UPON_DESTRUCTION_SPAWN_UNIT_AMOUNT_MIN);
    REQUIRE(INI_WordType("UponDestructionSpawnUnitAmountMax", eIniSection::STRUCTURES) == eIniWord::UPON_DESTRUCTION_SPAWN_UNIT_AMOUNT_MAX);
    REQUIRE(INI_WordType("UponDestructionSpawnUnitType", eIniSection::STRUCTURES) == eIniWord::UPON_DESTRUCTION_SPAWN_UNIT_TYPE);
}

TEST_CASE("INI_WordType - UNITS section numeric and boolean words", "[ini_parsing][words]")
{
    REQUIRE(INI_WordType("Bitmap", eIniSection::UNITS) == eIniWord::BITMAP);
    REQUIRE(INI_WordType("BitmapTop", eIniSection::UNITS) == eIniWord::BITMAP_TOP);
    REQUIRE(INI_WordType("Icon", eIniSection::UNITS) == eIniWord::ICON);
    REQUIRE(INI_WordType("HitPoints", eIniSection::UNITS) == eIniWord::HITPOINTS);
    REQUIRE(INI_WordType("Appetite", eIniSection::UNITS) == eIniWord::APPETITE);
    REQUIRE(INI_WordType("MoveSpeed", eIniSection::UNITS) == eIniWord::MOVESPEED);
    REQUIRE(INI_WordType("TurnSpeed", eIniSection::UNITS) == eIniWord::TURNSPEED);
    REQUIRE(INI_WordType("AttackFrequency", eIniSection::UNITS) == eIniWord::ATTACKFREQ);
    REQUIRE(INI_WordType("NextAttackFrequency", eIniSection::UNITS) == eIniWord::NEXTATTACKFREQ);
    REQUIRE(INI_WordType("IsHarvester", eIniSection::UNITS) == eIniWord::ISHARVESTER);
    REQUIRE(INI_WordType("FireTwice", eIniSection::UNITS) == eIniWord::FIRETWICE);
    REQUIRE(INI_WordType("IsInfantry", eIniSection::UNITS) == eIniWord::ISINFANTRY);
    REQUIRE(INI_WordType("Squishable", eIniSection::UNITS) == eIniWord::ISSQUISHABLE);
    REQUIRE(INI_WordType("CanSquish", eIniSection::UNITS) == eIniWord::CANSQUISH);
    REQUIRE(INI_WordType("IsAirborn", eIniSection::UNITS) == eIniWord::ISAIRBORN);
    REQUIRE(INI_WordType("AbleToCarry", eIniSection::UNITS) == eIniWord::ABLETOCARRY);
    REQUIRE(INI_WordType("FreeRoam", eIniSection::UNITS) == eIniWord::FREEROAM);
    REQUIRE(INI_WordType("MaxCredits", eIniSection::UNITS) == eIniWord::HARVESTLIMIT);
    REQUIRE(INI_WordType("HarvestSpeed", eIniSection::UNITS) == eIniWord::HARVESTSPEED);
    REQUIRE(INI_WordType("HarvestAmount", eIniSection::UNITS) == eIniWord::HARVESTAMOUNT);
    REQUIRE(INI_WordType("CanAttackAirUnits", eIniSection::UNITS) == eIniWord::CANATTACKAIRUNITS);
    REQUIRE(INI_WordType("CanAttackUnits", eIniSection::UNITS) == eIniWord::CANATTACKUNITS);
    REQUIRE(INI_WordType("AttackIsEnterStructure", eIniSection::UNITS) == eIniWord::ATTACK_IS_ENTER_STRUCTURE);
    REQUIRE(INI_WordType("DamageOnEnterStructure", eIniSection::UNITS) == eIniWord::DAMAGE_ON_ENTER_STRUCTURE);
}

TEST_CASE("INI_WordType - HOUSES section", "[ini_parsing][words]")
{
    REQUIRE(INI_WordType("ColorR", eIniSection::HOUSES) == eIniWord::RED);
    REQUIRE(INI_WordType("ColorG", eIniSection::HOUSES) == eIniWord::GREEN);
    REQUIRE(INI_WordType("ColorB", eIniSection::HOUSES) == eIniWord::BLUE);
    REQUIRE(INI_WordType("FirePower", eIniSection::HOUSES) == eIniWord::FIREPOWER);
    REQUIRE(INI_WordType("FireRate", eIniSection::HOUSES) == eIniWord::FIRERATE);
    REQUIRE(INI_WordType("StructPrice", eIniSection::HOUSES) == eIniWord::STRUCTPRICE);
    REQUIRE(INI_WordType("UnitPrice", eIniSection::HOUSES) == eIniWord::UNITPRICE);
    REQUIRE(INI_WordType("Speed", eIniSection::HOUSES) == eIniWord::SPEED);
    REQUIRE(INI_WordType("BuildSpeed", eIniSection::HOUSES) == eIniWord::BUILDSPEED);
    REQUIRE(INI_WordType("HarvestSpeed", eIniSection::HOUSES) == eIniWord::HARVESTSPEED);
    REQUIRE(INI_WordType("DumpSpeed", eIniSection::HOUSES) == eIniWord::DUMPSPEED);
}

TEST_CASE("INI_WordType - matching is case-insensitive", "[ini_parsing][words]")
{
    REQUIRE(INI_WordType("credits", eIniSection::HOUSEATREIDES) == eIniWord::CREDITS);
    REQUIRE(INI_WordType("CREDITS", eIniSection::HOUSEATREIDES) == eIniWord::CREDITS);
    REQUIRE(INI_WordType("hitpoints", eIniSection::UNITS) == eIniWord::HITPOINTS);
}

TEST_CASE("INI_WordType - words from another section are not recognised", "[ini_parsing][words]")
{
    REQUIRE(INI_WordType("Credits", eIniSection::MAP) == eIniWord::NONE);
    REQUIRE(INI_WordType("Width", eIniSection::HOUSEORDOS) == eIniWord::NONE);
    REQUIRE(INI_WordType("MoveSpeed", eIniSection::STRUCTURES) == eIniWord::NONE);
    REQUIRE(INI_WordType("Seed", eIniSection::NONE) == eIniWord::NONE);
}

TEST_CASE("INI_WordType - empty word gives NONE", "[ini_parsing][words]")
{
    REQUIRE(INI_WordType("", eIniSection::UNITS) == eIniWord::NONE);
    REQUIRE(INI_WordType("", eIniSection::MAP) == eIniWord::NONE);
}

TEST_CASE("Scenario line workflow - section header then key=value", "[ini_parsing][workflow]")
{
    auto sectionName = extractSectionName("[Atreides]");
    auto section = SCEN_INI_SectionType(sectionName);
    REQUIRE(section == eIniSection::HOUSEATREIDES);

    auto [left, right] = INI_SplitWord("Credits=2500");
    REQUIRE(INI_WordType(left, section) == eIniWord::CREDITS);
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
    REQUIRE(INI_WordType(left, eIniSection::BRIEFING) == eIniWord::REGIONTEXT);
    REQUIRE(removeQuote(right) == "Welcome to Arrakis");
}

TEST_CASE("Briefing workflow - scenario number line", "[ini_parsing][workflow]")
{
    auto [left, right] = INI_SplitWord("Number=3");
    REQUIRE(INI_WordType(left, eIniSection::SCEN) == eIniWord::NUMBER);
    REQUIRE(ToInt(right) == 3);
}
