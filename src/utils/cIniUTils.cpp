#include "iniDefine.h"
#include "d2tmc.h"

#include "utils/cIniUtils.h"
#include "utils/cLog.h"

const std::unordered_map<std::string, int> cIniUtils::sectionMap = {
    {"UNITS", INI_UNITS},
    {"SKIRMISH", INI_SKIRMISH},
    {"STRUCTURES", INI_STRUCTURES},
    {"REINFORCEMENTS", INI_REINFORCEMENTS},
    {"MAP", INI_MAP},
    {"BASIC", INI_BASIC},
    {"Atreides", INI_HOUSEATREIDES},
    {"Ordos", INI_HOUSEORDOS},
    {"Harkonnen", INI_HOUSEHARKONNEN},
    {"Sardaukar", INI_HOUSESARDAUKAR},
    {"Fremen", INI_HOUSEFREMEN},
    {"Mercenary", INI_HOUSEMERCENARY}
};

const std::unordered_map<std::string, int> cIniUtils::houseMap = {
    {"Atreides", ATREIDES},
    {"Harkonnen", HARKONNEN},
    {"Ordos", ORDOS},
    {"Sardaukar", SARDAUKAR},
    {"Mercenary", MERCENARY},
    {"Fremen", FREMEN},
    {"Corrino", CORRINO},
    {"General", GENERALHOUSE}
};

const std::unordered_map<std::string, int> cIniUtils::sectionTypeMap = {
    {"MAP", INI_MAP},
    {"SKIRMISH", INI_SKIRMISH},
    {"DESCRIPTION", INI_DESCRIPTION},
    {"SCEN", INI_SCEN},
    {"BRIEFING", INI_BRIEFING},
    {"WIN", INI_WIN},
    {"LOSE", INI_LOSE},
    {"ADVICE", INI_ADVICE},
    {"HOUSES", INI_HOUSES},
    {"UNITS", INI_UNITS},
    {"STRUCTURES", INI_STRUCTURES}
};

const std::unordered_map<std::string, std::string> cIniUtils::sceneFileMap = {
    {"HARVEST.WSA", "harvest"},
    {"IX.WSA", "ix"},
    {"SARDUKAR.WSA", "sardukar"},
    {"PALACE.WSA", "palace"},
    {"REPAIR.WSA", "repair"},
    {"HVYFTRY.WSA", "hvyftry"},
    {"HEADQRTS.WSA", "headqrts"},
    {"QUAD.WSA", "quad"},
    {"LTANK.WSA", "ltank"}
};

const std::unordered_map<std::string, int> cIniUtils::structureMap = {
    {"WINDTRAP", WINDTRAP},
    {"PALACE", PALACE},
    {"HEAVYFACTORY", HEAVYFACTORY},
    {"LIGHTFACTORY", LIGHTFACTORY},
    {"CONSTYARD", CONSTYARD},
    {"SILO", SILO},
    {"HIGHTECH", HIGHTECH},
    {"IX", IX},
    {"REPAIR", REPAIR},
    {"RADAR", RADAR},
    {"REFINERY", REFINERY},
    {"WOR", WOR},
    {"BARRACKS", BARRACKS},
    {"STARPORT", STARPORT},
    {"TURRET", TURRET},
    {"ROCKETTURRET", RTURRET},
    {"SLAB", SLAB1},
    {"4SLAB", SLAB4},
    {"WALL", WALL}
};

const std::unordered_map<std::string, int> cIniUtils::structureNameMap = {
    {"Const Yard", CONSTYARD},
    {"Palace", PALACE},
    {"Heavy Fctry", HEAVYFACTORY},
    {"Light Fctry", LIGHTFACTORY},
    {"Windtrap", WINDTRAP},
    {"Spice Silo", SILO},
    {"Hi-Tech", HIGHTECH},
    {"IX", IX},
    {"Repair", REPAIR},
    {"Outpost", RADAR},
    {"Refinery", REFINERY},
    {"WOR", WOR},
    {"Barracks", BARRACKS},
    {"Starport", STARPORT},
    {"Turret", TURRET},
    {"R-Turret", RTURRET}
};

const std::unordered_map<std::string, int> cIniUtils::unitNameMap = {
    {"Harvester", HARVESTER},
    {"Tank", TANK},
    {"COMBATTANK", TANK},
    {"Siege Tank", SIEGETANK},
    {"SIEGETANK", SIEGETANK},
    {"Launcher", LAUNCHER},
    {"Trooper", TROOPER},
    {"Troopers", TROOPERS},
    {"Sonic Tank", SONICTANK},
    {"SONICTANK", SONICTANK},
    {"Quad", QUAD},
    {"Trike", TRIKE},
    {"Raider Trike", RAIDER},
    {"RAIDER", RAIDER},
    {"Soldier", SOLDIER},
    {"Infantry", INFANTRY},
    {"Devastator", DEVASTATOR},
    {"Deviator", DEVIATOR},
    {"MCV", MCV},
    {"Trike", TRIKE},
    {"Soldier", SOLDIER},
    {"CarryAll", CARRYALL},
    {"Ornithopter", ORNITHOPTER},
    {"Sandworm", SANDWORM},
    {"Saboteur", SABOTEUR},
    {"ONEFREMEN", UNIT_FREMEN_ONE},
    {"THREEFREMEN", UNIT_FREMEN_THREE}
};

const std::unordered_map<int, char> cIniUtils::houseLetterMap = {
    {ATREIDES,   'a'},
    {HARKONNEN,  'h'},
    {ORDOS,      'o'},
    {SARDAUKAR,  's'},
    {MERCENARY,  'm'},
    {FREMEN,     'f'}
};

bool cIniUtils::caseInsCompare(const std::string& s1, const std::string& s2)
{
    if (s1.size() != s2.size()) return false;
    return std::equal(
        s1.begin(), s1.end(),
        s2.begin(),
        [](char a, char b) {
            return std::toupper(static_cast<unsigned char>(a)) == std::toupper(static_cast<unsigned char>(b));
        }
    );
}

std::string cIniUtils::getSceneFileToScene(const std::string& scenefile)
{
    // wsa / data
    for (const auto& [key, value] : sceneFileMap) {
        if (caseInsCompare(scenefile, key)) {
            return value;
        }
    }
    logbook(std::format("Failed to map dune 2 scenefile [{}] to a d2tm scene file.", scenefile));
    return "unknown";
}

int cIniUtils::getStructureType(const std::string& structureName)
{
    for (const auto& [key, value] : structureMap) {
        if (caseInsCompare(structureName, key)){
            return value;
        }
    }
    logbook(std::format("Could not find structure type for [{}]", structureName));
    return 0; // just in case some miracle happened, we need to go on and not crash everything.
}

// SCENxxxx.ini loader (for both DUNE II as for DUNE II - The Maker)
int cIniUtils::getUnitTypeFromString(const std::string& chunk)
{
    for (const auto& [key, value] : unitNameMap) {
        if (caseInsCompare(chunk, key)) {
            return value;
        }
    }
    logbook(std::format(
                "getUnitTypeFromChar could not determine what unit type '{}' is. Returning -1.", chunk));
    return -1;
}

int cIniUtils::getHouseFromString(const std::string& chunk)
{
    for (const auto& [key, value] : houseMap) {
        if (caseInsCompare(chunk, key)) {
            return value;
        }
    }
    logbook(std::format(
        "getHouseFromChar could not determine what house type '{}' is. Returning -1; this will probably cause problems.", chunk));
    return -1;
}

/**
 * Returns a string, containing the relative path to the scenario file for
 * the given house and region id.
 * @param iHouse
 * @param iRegion
 * @return
 */
std::string cIniUtils::getScenarioFileName(int iHouse, int iRegion)
{
    char cHouse = ' ';
    auto it = houseLetterMap.find(iHouse);
    if (it != houseLetterMap.end()) {
        cHouse = it->second;
    }
    return std::format("campaign/maps/scen{}{:03}.ini", cHouse, iRegion);
}

// Reads out section[], does a string compare and returns type id
int cIniUtils::getSectionType(const std::string& section, int last)
{
    for (const auto& [key, value] : sectionTypeMap) {
        if (cIniUtils::caseInsCompare(section, key)) {
            if (key == "STRUCTURES") {
                cLogger::getInstance()->log(LOG_ERROR, COMP_INIT, "Structure Section found", section);
            }
            return value;
        }
    }

    cLogger::getInstance()->log(LOG_ERROR, COMP_INIT, "No SECTION id found, assuming its an ID nested in section", section);
    return last;
}

// return ID of structure
int cIniUtils::getStructureTypeFromString(const std::string& structureStr)
{
    for (const auto& [key, value] : structureNameMap) {
        if (cIniUtils::caseInsCompare(structureStr, key)) {
            return value;
        }
    }
    logbook(std::format("Could not find structure: {}", structureStr));
    return CONSTYARD;
}

