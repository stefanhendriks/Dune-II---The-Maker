/*

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefan@fundynamic.com
  Website: http://dune2themaker.fundynamic.com

  2001 - 2022 (c) code by Stefan Hendriks

  */

#include "ini.h"
#include "iniDefine.h"
#include "utils/cIniUtils.h"

#include "d2tmc.h"
#include "data/gfxdata.h"
#include "definitions.h"
#include "gamestates/cSelectYourNextConquestState.h"
#include "managers/cDrawManager.h" // TODO: an ini file reader should not depend on drawing code
#include "map/cMapCamera.h"
#include "map/cMapEditor.h"
#include "map/MapGeometry.hpp"
#include "mentat/AbstractMentat.h"
#include "player/cPlayer.h"
#include "utils/cLog.h"
#include "utils/common.h"
#include "utils/cSeedMapGenerator.h"
#include "gameobjects/units/cReinforcements.h"
#include "utils/RNG.hpp"

#include <format>
#include <filesystem>
namespace fs=std::filesystem;

#include <string>
#include <algorithm>
#include <utility>
#include <unordered_map>


void INI_WordValueSENTENCE(char result[MAX_LINE_LENGTH], char value[256]);
int INI_GetPositionOfCharacter(char result[MAX_LINE_LENGTH], char c);

class cReinforcements;

bool INI_Scenario_Section_Units(int iHumanID, bool bSetUpPlayers, const int *iPl_credits, const int *iPl_house,
                                const int *iPl_quota, const char *linefeed);

bool INI_Scenario_Section_Structures(int iHumanID, bool bSetUpPlayers, const int *iPl_credits, const int *iPl_house,
                                     const int *iPl_quota, char *linefeed);

void INI_Scenario_Section_Reinforcements(int iHouse, const char *linefeed, cReinforcements *reinforcements);

void INI_Scenario_Section_MAP(int *blooms, int *fields, int wordtype, char *linefeed);

int INI_Scenario_Section_House(int wordtype, int iPlayerID, int *iPl_credits, int *iPl_quota, char *linefeed);

void INI_Scenario_Section_Basic(AbstractMentat *pMentat, int wordtype, char *linefeed);

void INI_Scenario_SetupPlayers(int iHumanID, const int *iPl_credits, const int *iPl_house, const int *iPl_quota);




// Reads out an entire sentence and returns it
void INI_Sentence(FILE *f, char result[MAX_LINE_LENGTH])
{
    if (fgets(result, MAX_LINE_LENGTH, f) == nullptr) {
        result[0] = '\0'; // EOF ou erreur
        return;
    }
    // get out \r and/or \n final for DOS/UNIX compatibility
    size_t len = strlen(result);
    while (len > 0 && (result[len-1] == '\n' || result[len-1] == '\r')) {
        result[--len] = '\0';
    }
}

/*********************************************************************************/
// Reads out INPUT , will check for a [ at [0] and then checks till ], it will fill section[]
// with the chars in between. So : [MAP] -> section = 'MAP'. Use function INI_SectionType(..)
// to get the correct ID for that.
void INI_Section(char input[MAX_LINE_LENGTH], char section[30])
{

    int pos = 0;
    int end_pos = -1;

    memset(section, '\0', strlen(section));

    // check if the first character is a '['
    if (input[0] == '[') {
        pos = 1; // Begin at character 1

        // find the ending ]
        while (pos < (MAX_LINE_LENGTH - 1)) {
            if (input[pos] == ']') {
                end_pos = pos - 1;
                break;
            }
            pos++;
        }

        if (end_pos > 1 && end_pos < 29) {
            for (int wc = 0; wc < end_pos; wc++) {
                section[wc] = input[wc + 1];
            }
            section[end_pos] = '\0'; // terminate string
        }
    }
}

// Reads out INPUT and will check for an '=' Everything at the left of the
// '=' IS a word and will be put in 'word[]'. Use function INI_WordType(char word[25]) to get
// the correct ID tag.
void INI_Word(char input[MAX_LINE_LENGTH], char word[30])
{
    int word_pos = INI_GetPositionOfCharacter(input, '=');

    memset(word, '\0', strlen(word));

    if (word_pos > -1 && word_pos < 28) {
        for (int wc = 0; wc < word_pos; wc++) {
            word[wc] = input[wc];
        }

        word[word_pos] = '\0'; // terminate string
    }
}

std::pair<std::string, std::string> INI_SplitWord(const std::string& input)
{
    auto pos = input.find('=');
    if (pos == std::string::npos) {
        return {input, ""}; // Pas de '=' trouvé, tout à gauche
    }
    std::string left = input.substr(0, pos);
    std::string right = input.substr(pos + 1);
    return {left, right};
}


/**
 * Return true when string "toFind" is in source string. Else return false.
 *
 * @param source
 * @param toFind
 * @return
 */
bool isInString(std::string source, std::string toFind)
{
    std::string::size_type loc = source.find(toFind, 0);
    if (loc == 0) {
        return true;
    }
    return false; // not found in string
}



// // Reads out word[], checks structure type, and returns actual source-id
// int INI_StructureType(char word[256])
// {
//     std::string wordAsString(word);
//     return cIniUtils::getStructureType(wordAsString);
// }

// Reads out word[], does a string compare and returns type id
int INI_WordType(char word[25], int section)
{
    logbook(std::format("Going to find word-type for [{}]", word));

    if (section == SEC_REGION) {
        if (cIniUtils::caseInsCompare(word, "Region"))            return WORD_REGION;
        if (cIniUtils::caseInsCompare(word, "RegionConquer"))     return WORD_REGIONCONQUER;
        if (cIniUtils::caseInsCompare(word, "House"))             return WORD_REGIONHOUSE;
        if (cIniUtils::caseInsCompare(word, "Text"))              return WORD_REGIONTEXT;
        if (cIniUtils::caseInsCompare(word, "Select"))            return WORD_REGIONSELECT;
        return WORD_NONE;
    }

    if (section == INI_SCEN ||
            section == INI_DESCRIPTION ||
            section == INI_BRIEFING ||
            section == INI_ADVICE ||
            section == INI_LOSE ||
            section == INI_WIN) {
        if (cIniUtils::caseInsCompare(word, "Number"))            return WORD_NUMBER;
        if (cIniUtils::caseInsCompare(word, "Text"))              return WORD_REGIONTEXT;
        return WORD_NONE;
    }

    if (section == INI_SKIRMISH) {
        if (cIniUtils::caseInsCompare(word, "Title"))             return WORD_MAPNAME;
        if (cIniUtils::caseInsCompare(word, "StartCell"))         return WORD_STARTCELL;
    }

    if (section == INI_GAME) {
        if (cIniUtils::caseInsCompare(word, "ModId"))             return WORD_MODID;
    }
    else if (section == INI_BASIC) {
        if (cIniUtils::caseInsCompare(word, "CursorPos"))         return WORD_FOCUS;
        if (cIniUtils::caseInsCompare(word, "BriefPicture"))      return WORD_BRIEFPICTURE;
        if (cIniUtils::caseInsCompare(word, "WinFlags"))          return WORD_WINFLAGS;
        if (cIniUtils::caseInsCompare(word, "LoseFlags"))         return WORD_LOSEFLAGS;
    }
    else if (section >= INI_HOUSEATREIDES && section <= INI_HOUSEMERCENARY) {
        if (cIniUtils::caseInsCompare(word, "Team"))              return WORD_TEAM;
        if (cIniUtils::caseInsCompare(word, "Credits"))           return WORD_CREDITS;
        if (cIniUtils::caseInsCompare(word, "Quota"))             return WORD_QUOTA;
        if (cIniUtils::caseInsCompare(word, "Brain"))             return WORD_BRAIN;
        if (cIniUtils::caseInsCompare(word, "House"))             return WORD_HOUSE;
        if (cIniUtils::caseInsCompare(word, "Focus"))             return WORD_FOCUS;
    }
    else if (section == INI_MAP) {
        // When reading [MAP], interpet the 'width' and 'height' for default width and height for the Map Editor
        if (cIniUtils::caseInsCompare(word, "Width"))             return WORD_MAPWIDTH;
        if (cIniUtils::caseInsCompare(word, "Height"))            return WORD_MAPHEIGHT;
        if (cIniUtils::caseInsCompare(word, "Seed"))              return WORD_MAPSEED;
        if (cIniUtils::caseInsCompare(word, "Bloom"))             return WORD_MAPBLOOM;
        if (cIniUtils::caseInsCompare(word, "Field"))             return WORD_MAPFIELD;
    }
    else if (section == INI_BULLETS) {
        if (cIniUtils::caseInsCompare(word, "Bitmap"))            return WORD_BITMAP;
        if (cIniUtils::caseInsCompare(word, "BitmapExplosion"))   return WORD_BITMAP_DEAD;
        if (cIniUtils::caseInsCompare(word, "BitmapWidth"))       return WORD_BITMAP_WIDTH;
        if (cIniUtils::caseInsCompare(word, "BitmapFrames"))      return WORD_BITMAP_FRAMES;
        if (cIniUtils::caseInsCompare(word, "BitmapExplFrames"))  return WORD_BITMAP_DEADFRAMES;
        if (cIniUtils::caseInsCompare(word, "Damage"))            return WORD_DAMAGE;
        if (cIniUtils::caseInsCompare(word, "Definition"))        return WORD_DEFINITION;
        if (cIniUtils::caseInsCompare(word, "Sound"))             return WORD_SOUND;
    }
    else if (section == INI_STRUCTURES) {
        if (cIniUtils::caseInsCompare(word, "HitPoints"))         return WORD_HITPOINTS;
        if (cIniUtils::caseInsCompare(word, "FixPoints"))         return WORD_FIXHP;
        if (cIniUtils::caseInsCompare(word, "PowerDrain"))        return WORD_POWERDRAIN;
        if (cIniUtils::caseInsCompare(word, "PowerGive"))         return WORD_POWERGIVE;
        if (cIniUtils::caseInsCompare(word, "Cost"))              return WORD_COST;
        if (cIniUtils::caseInsCompare(word, "BuildTime"))         return WORD_BUILDTIME;
    }
    else if (section == INI_UNITS) {
        if (cIniUtils::caseInsCompare(word, "Bitmap"))            return WORD_BITMAP;
        if (cIniUtils::caseInsCompare(word, "BitmapTop"))         return WORD_BITMAP_TOP;
        if (cIniUtils::caseInsCompare(word, "Icon"))              return WORD_ICON;
        if (cIniUtils::caseInsCompare(word, "BitmapWidth"))       return WORD_BITMAP_WIDTH;
        if (cIniUtils::caseInsCompare(word, "BitmapHeight"))      return WORD_BITMAP_HEIGHT;
        if (cIniUtils::caseInsCompare(word, "HitPoints"))         return WORD_HITPOINTS;
        if (cIniUtils::caseInsCompare(word, "Appetite"))          return WORD_APPETITE;
        if (cIniUtils::caseInsCompare(word, "Cost"))              return WORD_COST;
        if (cIniUtils::caseInsCompare(word, "BulletType"))        return WORD_BULLETTYPE;
        if (cIniUtils::caseInsCompare(word, "MoveSpeed"))         return WORD_MOVESPEED;
        if (cIniUtils::caseInsCompare(word, "TurnSpeed"))         return WORD_TURNSPEED;
        // Attack frequency (todo: wording, it should be more like "delay" or "fireRate")
        if (cIniUtils::caseInsCompare(word, "AttackFrequency"))   return WORD_ATTACKFREQ;
        // Next Attack frequency (if applicable) (todo: wording, it should be more like "delay" or "fireRate")
        if (cIniUtils::caseInsCompare(word, "NextAttackFrequency"))   return WORD_NEXTATTACKFREQ;
        if (cIniUtils::caseInsCompare(word, "Sight"))            return WORD_SIGHT;
        if (cIniUtils::caseInsCompare(word, "Range"))            return WORD_RANGE;
        if (cIniUtils::caseInsCompare(word, "BuildTime"))        return WORD_BUILDTIME;
        if (cIniUtils::caseInsCompare(word, "Description"))      return WORD_DESCRIPTION;
        // BOOLEANS
        if (cIniUtils::caseInsCompare(word, "IsHarvester"))       return WORD_ISHARVESTER;
        if (cIniUtils::caseInsCompare(word, "FireTwice"))         return WORD_FIRETWICE;
        if (cIniUtils::caseInsCompare(word, "IsInfantry"))        return WORD_ISINFANTRY;
        if (cIniUtils::caseInsCompare(word, "Squishable"))        return WORD_ISSQUISHABLE;
        if (cIniUtils::caseInsCompare(word, "CanSquish"))         return WORD_CANSQUISH;
        if (cIniUtils::caseInsCompare(word, "IsAirborn"))         return WORD_ISAIRBORN;
        if (cIniUtils::caseInsCompare(word, "AbleToCarry"))       return WORD_ABLETOCARRY;
        if (cIniUtils::caseInsCompare(word, "FreeRoam"))          return WORD_FREEROAM;
        if (cIniUtils::caseInsCompare(word, "Producer"))          return WORD_PRODUCER;
        if (cIniUtils::caseInsCompare(word, "MaxCredits"))        return WORD_HARVESTLIMIT;
        if (cIniUtils::caseInsCompare(word, "HarvestSpeed"))      return WORD_HARVESTSPEED;
        if (cIniUtils::caseInsCompare(word, "HarvestAmount"))     return WORD_HARVESTAMOUNT;
    }
    else if (section == INI_STRUCTURES) {
        if (strlen(word) > 1) {
            if (cIniUtils::caseInsCompare(word, "PreBuild"))      return WORD_PREBUILD;
            if (cIniUtils::caseInsCompare(word, "Description"))   return WORD_DESCRIPTION;
            if (cIniUtils::caseInsCompare(word, "Power"))         return WORD_POWER;        // What power it takes
        }
        else
            return WORD_NONE;
    }
    else if (section == INI_HOUSES) {
        // each house has properties..
        if (cIniUtils::caseInsCompare(word, "ColorR"))            return WORD_RED;
        if (cIniUtils::caseInsCompare(word, "ColorG"))            return WORD_GREEN;
        if (cIniUtils::caseInsCompare(word, "ColorB"))            return WORD_BLUE;
        // and specific stuff:
        if (cIniUtils::caseInsCompare(word, "FirePower"))         return WORD_FIREPOWER;
        if (cIniUtils::caseInsCompare(word, "FireRate"))          return WORD_FIRERATE;
        if (cIniUtils::caseInsCompare(word, "StructPrice"))       return WORD_STRUCTPRICE;
        if (cIniUtils::caseInsCompare(word, "UnitPrice"))         return WORD_UNITPRICE;
        if (cIniUtils::caseInsCompare(word, "Speed"))             return WORD_SPEED;
        if (cIniUtils::caseInsCompare(word, "BuildSpeed"))        return WORD_BUILDSPEED;
        if (cIniUtils::caseInsCompare(word, "HarvestSpeed"))      return WORD_HARVESTSPEED;
        if (cIniUtils::caseInsCompare(word, "DumpSpeed"))         return WORD_DUMPSPEED;
    }

    logbook(std::format("Could not find word-type for [{}]", word));
    return WORD_NONE;
}


// Scenario section types
int SCEN_INI_SectionType(char section[30])
{
    if (strcmp(section, "UNITS") == 0)
        return INI_UNITS;

    if (strcmp(section, "SKIRMISH") == 0)
        return INI_SKIRMISH;

    if (strcmp(section, "STRUCTURES") == 0)
        return INI_STRUCTURES;

    if (strcmp(section, "REINFORCEMENTS") == 0)
        return INI_REINFORCEMENTS;

    if (strcmp(section, "MAP") == 0)
        return INI_MAP;

    if (strcmp(section, "BASIC") == 0)
        return INI_BASIC;

    if (strcmp(section, "Atreides") == 0)
        return INI_HOUSEATREIDES;

    if (strcmp(section, "Ordos") == 0)
        return INI_HOUSEORDOS;

    if (strcmp(section, "Harkonnen") == 0)
        return INI_HOUSEHARKONNEN;

    if (strcmp(section, "Sardaukar") == 0)
        return INI_HOUSESARDAUKAR;

    if (strcmp(section, "Fremen") == 0)
        return INI_HOUSEFREMEN;

    if (strcmp(section, "Mercenary") == 0)
        return INI_HOUSEMERCENARY;

    return -1;
}


// GAME Section Types
int GAME_INI_SectionType(char section[30], int last)
{

// if (strcmp(section, "BULLETS") == 0)
//   return INI_BULLETS;

    if (strcmp(section, "UNITS") == 0)
        return INI_UNITS;

    if (strcmp(section, "STRUCTURES") == 0)
        return INI_STRUCTURES;

    // When nothing found; we assume its just a new ID tag for some unit or structure
    // Therefor we return the last known SECTION ID so we can assign the proper WORD ID's
    return last;
}



// Reads out 'result' and will return the value after the '='. Returns float.
// UGLY COPY/PASTE OF INI_WORDVALUEINT function, because we will completely
// rewrite this ini parsing abomination somewhere in the future.
float INI_WordValueFloat(char result[MAX_LINE_LENGTH], float defaultValue)
{
    int pos = 0;
    int is_pos = -1;

    while (pos < (MAX_LINE_LENGTH - 1)) {
        if (result[pos] == '=') {
            is_pos = pos;
            break;
        }
        pos++;
    }

    if (is_pos > -1) {
        // Whenever the IS (=) position is known, we make a number out of 'IS_POS' till the next empty
        // space.
        int end_pos = -1;

        while (pos < (MAX_LINE_LENGTH - 1)) {
            if (result[pos] == '\0') {
                end_pos = pos;
                break;
            }
            pos++;
        }

        // End position found!
        if (end_pos > -1) {
            // We know the END position. We will use that piece of string to read out a number.
            char number[10];

            // clear out entire string
            for (int i = 0; i < 10; i++)
                number[i] = '\0';

            // Copy the part to 'number', Make sure we won't get outside the array of the character.
            int cp = is_pos + 1;
            int c = 0;
            while (cp < end_pos) {
                number[c] = result[cp];
                c++;
                cp++;
                if (c > 9)
                    break;
            }
            return atof(number);
        }
        // nothing here, so we return nullptr at the end
    }

    return defaultValue; // no value found, return this
}

// Reads out 'result' and will return the value after the '='. Returns integer.
// For CHAR returns see "INI_WordValueCHAR(char result[80]);
int INI_WordValueINT(char result[MAX_LINE_LENGTH])
{
    int pos = 0;
    int is_pos = -1;

    while (pos < (MAX_LINE_LENGTH - 1)) {
        if (result[pos] == '=') {
            is_pos = pos;
            break;
        }
        pos++;
    }

    if (is_pos > -1) {
        // Whenever the IS (=) position is known, we make a number out of 'IS_POS' till the next empty
        // space.
        int end_pos = -1;

        while (pos < (MAX_LINE_LENGTH - 1)) {
            if (result[pos] == '\0') {
                end_pos = pos;
                break;
            }
            pos++;
        }

        // End position found!
        if (end_pos > -1) {
            // We know the END position. We will use that piece of string to read out a number.
            char number[10];

            // clear out entire string
            for (int i = 0; i < 10; i++)
                number[i] = '\0';

            // Copy the part to 'number', Make sure we won't get outside the array of the character.
            int cp = is_pos + 1;
            int c = 0;
            while (cp < end_pos) {
                number[c] = result[cp];
                c++;
                cp++;
                if (c > 9)
                    break;
            }
            return atoi(number);
        }
        // nothing here, so we return nullptr at the end
    }

    return 0; // No value, return 0
}


void INI_WordValueSENTENCE(char result[MAX_LINE_LENGTH], char value[256])
{
    int pos = 0;
    int is_pos = -1;

    // clear out entire string
    memset(value, 0, strlen(value));

    for (int i = 0; i < MAX_LINE_LENGTH; i++)
        value[i] = '\0';


    while (pos < (MAX_LINE_LENGTH - 1)) {
        if (result[pos] == '"') {
            is_pos = pos;
            break;
        }
        pos++;
    }

    if (is_pos > -1) {
        // Whenever the IS (=) position is known, we make a number out of 'IS_POS' till the next empty
        // space.
        int end_pos = -1;
        pos++;

        while (pos < (MAX_LINE_LENGTH - 1)) {
            if (result[pos] == '"') {
                end_pos = pos;
                break;
            }
            pos++;
        }

        // End position found!
        if (end_pos > -1) {
            // We know the END position. We will use that piece of string to read out a number.

            // Copy the part to 'value', Make sure we won't get outside the array of the character.
            int cp = is_pos + 1;
            int c = 0;
            while (cp < end_pos) {
                value[c] = result[cp];
                c++;
                cp++;
                if (c > 254)
                    break;
            }
        }
    }
}

int INI_GetPositionOfCharacter(char result[MAX_LINE_LENGTH], char c)
{
    std::string resultString(result);
    return resultString.find_first_of(c, 0);
}


/**
 * Return the part after the = sign as string.
 *
 * @param result
 * @return
 */
std::string INI_WordValueString(char result[MAX_LINE_LENGTH])
{
    std::string resultAsString(result);
    int isPos = INI_GetPositionOfCharacter(result, '=');
    return resultAsString.substr(isPos + 1);
}


// Reads out 'result' and will return the value after the '='. Returns nothing but will put
// the result in 'value[25]'. Max argument may be 256 characters!
void INI_WordValueCHAR(char result[MAX_LINE_LENGTH], char value[256])
{
    int pos = 0;
    int is_pos = -1;

    // clear out entire string
    memset(value, 0, strlen(value));
    for (int i = 0; i < 256; i++) {
        value[i] = '\0';
    }

    while (pos < (MAX_LINE_LENGTH - 1)) {
        if (result[pos] == '=') {
            is_pos = pos;
            break;
        }
        pos++;
    }

    if (is_pos > -1) {
        // Whenever the IS (=) position is known, we make a number out of 'IS_POS' till the next empty
        // space.
        int end_pos = -1;

        while (pos < (MAX_LINE_LENGTH - 1)) {
            if (result[pos] == '\0' || result[pos] == '\n') {
                end_pos = (pos - 1);
                break;
            }
            pos++;
        }

        // End position found!
        if (end_pos > -1) {
            // We know the END position. We will use that piece of string to read out a number.

            // Copy the part to 'value', Make sure we won't get outside the array of the character.
            int cp = is_pos + 1;
            int c = 0;
            while (cp <= end_pos) {
                value[c] = result[cp];
                c++;
                cp++;
                if (c > 80) {
                    break;
                }
            }
        }
    }
}


// Reads out 'result' and will return TRUE when its 'TRUE' or FALSE when its 'FALSE' , else
// returns nullptr
bool INI_WordValueBOOL(char result[MAX_LINE_LENGTH])
{
    // use INI_WordValueCHAR to know if its 'true'
    char val[256];
    INI_WordValueCHAR(result, val);

    // When its TRUE , return true
    return cIniUtils::caseInsCompare(val, "true");
}



/**
 * Create seed map.
 *
 * @param seed
 */
void INI_Load_seed(int seed)
{

    logbook(std::format("Generating seed map with seed {}.", seed));

    auto seedGenerator = cSeedMapGenerator(seed);

    auto seedMap = seedGenerator.generateSeedMap();
    logbook("Seedmap generated");

    auto mapEditor = cMapEditor(global_map);
    for (int mapY = 0; mapY < 64; mapY++) {
        for (int mapX = 0; mapX < 64; mapX++) {
            int type = seedMap.getCellType(mapX, mapY);
            int iCell = global_map.getGeometry().makeCell(mapX, mapY);
            mapEditor.createCell(iCell, type, 0);
        }
    }

    logbook("Seedmap converted into D2TM map.");
}


// Load
/**
 * Return true when line starts with:
 * ; # // \n \0
 *
 * @param linefeed
 * @return
 */
bool isCommentLine(const std::string& linefeed)
{
    if (linefeed.empty()) return true;
    char first = linefeed[0];
    if (first == ';' || first == '#' || first == '\n' || first == '\0') return true;
    if (first == '/' && linefeed.size() > 1 && linefeed[1] == '/') return true;
    return false;
}

/**
 * Return the name of the directory to look for by house id.
 * (ie iHouse == ATREIDES) returns "atreides"
 * @param iHouse
 * @return
 */
std::string INI_GetHouseDirectoryName(int iHouse)
{
    using namespace std::string_view_literals;
    switch (iHouse) {
        case ATREIDES:   return "atreides";
        case HARKONNEN:  return "harkonnen";
        case SARDAUKAR:  return "sardaukar";
        case ORDOS:      return "ordos";
        case FREMEN:     return "fremen";
        case MERCENARY:  return "mercenary";
        default:         return "unknown";
    }
}

void INI_Load_Regionfile(int iHouse, int iMission, cSelectYourNextConquestState *selectYourNextConquestState)
{
    auto filename = std::format("campaign/{}/mission{}.ini", INI_GetHouseDirectoryName(iHouse), iMission);
    cLogger::getInstance()->log(LOG_INFO, COMP_REGIONINI, "Opening mission file", filename);

    std::ifstream file(filename);
    if (!file.is_open()) {
        logbook("[CAMPAIGN] Error, could not open file");
        return;
    }

    int wordtype = WORD_NONE;
    int iRegionIndex = -1;
    int iRegionNumber = -1;
    int iRegionConquer = -1;

    std::string line;
    while (std::getline(file, line)) {
        // Nettoyage des fins de ligne DOS/UNIX
        line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());

        // Sauter les lignes de commentaire ou vides
        if (line.empty() || isCommentLine(line)) {
            continue;
        }

        char lineword[30] = {0};

        INI_Word(const_cast<char *>(line.c_str()), lineword);
        wordtype = INI_WordType(lineword, SEC_REGION);

        if (wordtype == WORD_REGION) {
            iRegionNumber = -1;
            iRegionConquer = -1;
            iRegionNumber = INI_WordValueINT(const_cast<char *>(line.c_str())) - 1;
        }
        else if (wordtype == WORD_REGIONCONQUER) {
            iRegionNumber = -1;
            iRegionConquer = -1;
            iRegionIndex++;
            iRegionConquer = INI_WordValueINT(const_cast<char *>(line.c_str())) - 1;
            selectYourNextConquestState->setRegionConquer(iRegionIndex, iRegionConquer);
        }

        if (iRegionIndex > -1 || iRegionNumber > -1) {
            if (wordtype == WORD_REGIONHOUSE) {
                char cHouseRegion[256] = {0};
                INI_WordValueCHAR(const_cast<char *>(line.c_str()), cHouseRegion);

                logbook("Region house");
                int iH = cIniUtils::getHouseFromString(cHouseRegion);

                if (iRegionNumber > -1) {
                    world[iRegionNumber].iHouse = iH;
                    world[iRegionNumber].iAlpha = 255;
                }

                if (iRegionConquer > -1) {
                    selectYourNextConquestState->setRegionHouse(iRegionIndex, iH);
                }
            }

            if (wordtype == WORD_REGIONTEXT && iRegionConquer > -1 && iRegionIndex > -1) {
                char cHouseText[256] = {0};
                INI_WordValueSENTENCE(const_cast<char *>(line.c_str()), cHouseText);
                selectYourNextConquestState->setRegionText(iRegionIndex, cHouseText);
            }

            if (wordtype == WORD_REGIONSELECT) {
                if (iRegionNumber > -1) {
                    world[iRegionNumber].bSelectable = INI_WordValueBOOL(const_cast<char *>(line.c_str()));
                    world[iRegionNumber].iAlpha = 1;
                }
            }
        }
    }

    logbook("[CAMPAIGN] Done");
}

/**
 * Taken the region conquered by player, in sequential form (meaning, the 1st region the
 * player conquers, corresponds with techlevel 1. While the 8th, 9th or 10th region correspond
 * with techlevel 4.
 *
 * This assumes that the player conquers the world in a fixed set of regions.
 *
 * TODO: Make this moddable.
 *
 * @param iRegion
 * @return
 */
int getTechLevelByRegion(int iRegion)
{
    if (iRegion == 1) return 1;
    if (iRegion == 2 || iRegion == 3 || iRegion == 4) return 2;
    if (iRegion == 5 || iRegion == 6 || iRegion == 7) return 3;
    if (iRegion == 8 || iRegion == 9 || iRegion == 10) return 4;
    if (iRegion == 11 || iRegion == 12 || iRegion == 13) return 5;
    if (iRegion == 14 || iRegion == 15 || iRegion == 16) return 6;
    if (iRegion == 17 || iRegion == 18 || iRegion == 19) return 7;
    if (iRegion == 20 || iRegion == 21) return 8;
    return 9;
}


void INI_Load_scenario(int iHouse, int iRegion, AbstractMentat *pMentat, cReinforcements *reinforcements)
{
    game.m_skirmish = false;
    game.missionInit();

    std::string filename = cIniUtils::getScenarioFileName(iHouse, iRegion);

    game.m_mission = getTechLevelByRegion(iRegion);

    logbook(std::format("[SCENARIO] '{}' (Mission {})", filename, game.m_mission));
    logbook("[SCENARIO] Opening file");

    // declare some temp fields while reading the scenario file.
    int blooms[30], fields[30];
    memset(blooms, -1, sizeof(blooms));
    memset(fields, -1, sizeof(fields));

    FILE *stream;
    int section = INI_NONE;
    int wordtype = WORD_NONE;
    int iPlayerID = -1;
    int iHumanID = -1;
    bool bSetUpPlayers = true;
    int iPl_credits[MAX_PLAYERS];
    int iPl_house[MAX_PLAYERS];
    int iPl_quota[MAX_PLAYERS];

    memset(iPl_credits, 0, sizeof(iPl_credits));
    memset(iPl_house, -1, sizeof(iPl_house));
    memset(iPl_quota, 0, sizeof(iPl_quota));

    auto mapEditor = cMapEditor(global_map);

    if ((stream = fopen(filename.c_str(), "r+t")) != nullptr) {
        char linefeed[MAX_LINE_LENGTH];
        char lineword[30];
        char linesection[30];

        memset(lineword, '\0', sizeof(lineword));
        memset(linesection, '\0', sizeof(linesection));
        memset(linefeed, '\0', sizeof(linefeed));

        // infinite loop baby
        while (!feof(stream)) {
            INI_Sentence(stream, linefeed);

            // Linefeed contains a string of 1 sentence. Whenever the first character is a commentary
            // character (which is "//", ";" or "#"), or an empty line, then skip it
            if (isCommentLine(linefeed)) continue;   // Skip

            // Every line is checked for a new section.
            INI_Section(linefeed, linesection);

            // line is not starting empty and section is found
            if (linesection[0] != '\0' && strlen(linesection) > 1) {
                int sectionType = SCEN_INI_SectionType(linesection);
                if (sectionType > -1) {
                    // found a section
                    section = sectionType;

                    logbook(std::format("[SCENARIO] found section '{}', resulting in section id [{}]", linesection, section));

                    if (section >= INI_HOUSEATREIDES && section <= INI_HOUSEMERCENARY) {
                        iPlayerID++;

                        if (iPlayerID > (MAX_PLAYERS - 1)) {
                            iPlayerID = (MAX_PLAYERS - 1);
                        }

                        int house = -1;
                        if (section == INI_HOUSEATREIDES) house = ATREIDES;
                        if (section == INI_HOUSEORDOS) house = ORDOS;
                        if (section == INI_HOUSEHARKONNEN) house = HARKONNEN;
                        if (section == INI_HOUSEMERCENARY) house = MERCENARY;
                        if (section == INI_HOUSEFREMEN) house = FREMEN;
                        if (section == INI_HOUSESARDAUKAR) house = SARDAUKAR;

                        iPl_house[iPlayerID] = house;

                        logbook(std::format("[SCENARIO] Setting house to [{}] for playerId [{}]", iPl_house[iPlayerID], iPlayerID));
                    }
                }
                continue; // next line
            }

            // Okay, we found a new section; if its NOT [GAME] then we remember this one!
            if (section != INI_NONE) {
                INI_Word(linefeed, lineword);
                wordtype = INI_WordType(lineword, section);
            }

            if (section == INI_BASIC) {
                INI_Scenario_Section_Basic(pMentat, wordtype, linefeed);
            }

            // Dune 2 house found, load player data
            if (section >= INI_HOUSEATREIDES && section <= INI_HOUSEMERCENARY) {
                int result = INI_Scenario_Section_House(wordtype, iPlayerID, iPl_credits, iPl_quota, linefeed);
                if (result > -1) {
                    iHumanID = result;
                }
            }

            if (section == INI_MAP) {
                INI_Scenario_Section_MAP(blooms, fields, wordtype, linefeed);
            }
            else if (section == INI_UNITS) {
                bSetUpPlayers = INI_Scenario_Section_Units(iHumanID, bSetUpPlayers, iPl_credits, iPl_house, iPl_quota, linefeed);
            }
            else if (section == INI_STRUCTURES) {
                bSetUpPlayers = INI_Scenario_Section_Structures(iHumanID, bSetUpPlayers, iPl_credits, iPl_house, iPl_quota, linefeed);
            }
            else if (section == INI_REINFORCEMENTS) {
                INI_Scenario_Section_Reinforcements(iHouse, linefeed, reinforcements);
            }
            wordtype = WORD_NONE;
        }

        fclose(stream);

        mapEditor.smoothMap();

        // now add the spice blooms! :)
        for (int iB = 0; iB < 30; iB++) {
            // when
            if (blooms[iB] > -1) {
                //   map.cell[blooms[iB]].tile = BLOOM;

                if (game.isDebugMode()) {
                    logbook(std::format("[SCENARIO] Placing spice BLOOM at cell : {}", blooms[iB]));
                }

                mapEditor.createCell(blooms[iB], TERRAIN_BLOOM, 0);

            }
        }

        // At this point, show list of unit types

        // now add the fields
        for (int iB = 0; iB < 30; iB++) {
            // when
            if (fields[iB] > -1) {
                if (game.isDebugMode()) {
                    logbook(std::format("[SCENARIO] Placing spice FIELD at cell : {}", fields[iB]));
                }
                mapEditor.createRandomField(fields[iB], TERRAIN_SPICE, 25 + (RNG::rnd(50)));
            }

        }

        logbook("[SCENARIO] Done reading");
    }

    mapEditor.smoothMap();
    global_map.setDesiredAmountOfWorms(players[AI_WORM].getAmountOfUnitsForType(SANDWORM));
}

void INI_Scenario_Section_Basic(AbstractMentat *pMentat, int wordtype, char *linefeed)
{
    if (wordtype == WORD_BRIEFPICTURE) {
        // Load name, and load proper briefingpicture
        std::string scenefile = INI_WordValueString(linefeed);
        std::string scene = cIniUtils::getSceneFileToScene(scenefile);

        scene = cIniUtils::getSceneFileToScene(scenefile);

        if (!isInString(scene, "unknown")) {
            pMentat->loadScene(scene);
        }
    }
    else if (wordtype == WORD_FOCUS) {
        int focusCell = INI_WordValueINT(linefeed);
        players[0].setFocusCell(focusCell);
        mapCamera->centerAndJumpViewPortToCell(focusCell);
    }
    else if (wordtype == WORD_WINFLAGS) {
        game.setWinFlags(INI_WordValueINT(linefeed));
    }
    else if (wordtype == WORD_LOSEFLAGS) {
        game.setLoseFlags(INI_WordValueINT(linefeed));
    }
}

int INI_Scenario_Section_House(int wordtype, int iPlayerID, int *iPl_credits, int *iPl_quota, char *linefeed)
{
    int iHumanID = -1;
    logbook(std::format("Section is between atreides and mercenary, the playerId is [{}]. WordType is [{}]",
                        iPlayerID, wordtype));
    // link house (found, because > -1)
    if (iPlayerID > -1) {
        if (wordtype == WORD_BRAIN) {
            char cBrain[256];
            memset(cBrain, 0, sizeof(cBrain));
            INI_WordValueCHAR(linefeed, cBrain);

            logbook(std::format("Brain is [{}]", cBrain));

            // We know the human brain now, this should be player 0 in our game (!?)...
            if (strcmp(cBrain, "Human") == 0) {
                logbook(std::format("Found human player for id [{}]", iPlayerID));
                iHumanID = iPlayerID;
            }
            else {
                logbook("This brain is not human...");
            }
        }
        else if (wordtype == WORD_CREDITS) {
            int credits = INI_WordValueINT(linefeed) - 1;
            logbook(std::format("Set credits for player id [{}] to [{}]", iPlayerID, credits));

            iPl_credits[iPlayerID] = credits;
        }
        else if (wordtype == WORD_QUOTA) {
            iPl_quota[iPlayerID] = INI_WordValueINT(linefeed);
        }
    }
    return iHumanID;
}

void INI_Scenario_Section_MAP(int *blooms, int *fields, int wordtype, char *linefeed)
{
    global_map.init(64, 64);

    // original dune 2 maps have 64x64 maps
    if (wordtype == WORD_MAPSEED) {
        logbook("[SCENARIO] -> [MAP] Seed=");
        INI_Load_seed(INI_WordValueINT(linefeed));
    }

    // Loaded before SEED
    if (wordtype == WORD_MAPBLOOM) {
        // This should put spice blooms in our array
        // Logic: read till next "," , then use that number to determine
        // where the bloom will be (as cell nr)
        logbook("[SCENARIO] -> [MAP] Bloom=");

        int iBloomID = 0;
        int iStringID = 6;    // B L O O M = <6>
        int iWordID = 0;

        char word[10];
        memset(word, 0, sizeof(word)); // clear string

        for (; iStringID < MAX_LINE_LENGTH; iStringID++) {
            // until we encounter a "," ...

            char letter[1];
            letter[0] = '\0';
            letter[0] = linefeed[iStringID];

            // its a comma!
            if (letter[0] == ',' || letter[0] == '\0' || letter[0] == '\n') {
                // from prevID TILL now is a number
                iWordID = 0;

                int original_dune2_cell = atoi(word);
                int d2tm_cell = -1;

                int iCellX = (original_dune2_cell - ((original_dune2_cell / 64) * 64));
                int iCellY = (original_dune2_cell / 64);

                // Now recalculate it
                d2tm_cell = global_map.getGeometry().makeCell(iCellX, iCellY);
                blooms[iBloomID] = d2tm_cell;
                memset(word, 0, sizeof(word)); // clear string

                if (iBloomID < 29)
                    iBloomID++;

                if (letter[0] == '\0' || letter[0] == '\n')
                    break; // get out
            }
            else {
                word[iWordID] = letter[0]; // copy
                if (iWordID < 9)
                    iWordID++;

            }
        }
    }
    // Loaded before SEED
    else if (wordtype == WORD_MAPFIELD) {
        // This should put spice blooms in our array
        // Logic: read till next "," , then use that number to determine
        // where the bloom will be (as cell nr)

        logbook("[SCENARIO] -> [MAP] Field=");
        int iFieldID = 0;
        int iStringID = 6;    // F I E L D = <6>
        int iWordID = 0;

        char word[10];
        memset(word, 0, sizeof(word)); // clear string

        for (; iStringID < MAX_LINE_LENGTH; iStringID++) {
            // until we encounter a "," ...

            char letter[1];
            letter[0] = '\0';
            letter[0] = linefeed[iStringID];

            // its a comma!
            if (letter[0] == ',' || letter[0] == '\0' || letter[0] == '\n') {
                // from prevID TILL now is a number
                iWordID = 0;

                int original_dune2_cell = atoi(word);
                int d2tm_cell = -1;

                int iCellX = (original_dune2_cell - ((original_dune2_cell / 64) * 64));
                int iCellY = (original_dune2_cell / 64);

                // Now recalculate it
                d2tm_cell = global_map.getGeometry().makeCell(iCellX, iCellY);
                fields[iFieldID] = d2tm_cell;
                memset(word, 0, sizeof(word)); // clear string

                if (iFieldID < 29)
                    iFieldID++;

                if (letter[0] == '\0' || letter[0] == '\n')
                    break; // get out
            }
            else {
                word[iWordID] = letter[0]; // copy
                if (iWordID < 9)
                    iWordID++;

            }
        }
    }
}

void INI_Scenario_Section_Reinforcements(int iHouse, const char *linefeed, cReinforcements *reinforcements)
{
    logbook("[SCENARIO] -> REINFORCEMENTS");

    int iPart = -1; /*
                0 = Controller
                1 = Type
                2 = HP
                3 = Cell
                */

    // Skip ID= part. It is just for fun there.
    int playerId, unitType, delayInMinutes, targetCell;
    playerId = unitType = delayInMinutes = targetCell = -1;

    char chunk[25];
    bool bClearChunk = true;
    bool bSkipped = false;
    int iC = -1;

    for (int c = 0; c < MAX_LINE_LENGTH; c++) {
        // clear chunk
        if (bClearChunk) {
            memset(chunk, 0, sizeof(chunk));
            //for (int ic=0; ic < 25; ic++)
            //	chunk[ic] = '\0';
            iC = 0;
            bClearChunk = false;
        }

        // Fill in chunk
        if (iC < 25 && bSkipped && linefeed[c] != ',') {
            chunk[iC] = linefeed[c];
            iC++;
        }

        // , means next part. A ' ' means end

        // Example:
        // 1=Harkonnen,Troopers,Enemybase,11
        // <ID>=<House>,<UnitType>,<DropLocation>,<Time>
        // <Time> may be postfixed with a '+' meaning it should repeat infinitely. Unfortunately Dune II
        // has a bug ignoring the '+'. (but we can fix this)

        bool plusDetected = (linefeed[c] == '+'); // plus has special meaning
        if (linefeed[c] == ',' || linefeed[c] == '\0' || plusDetected) {
            iPart++;

            if (iPart == 0) {
                int iHouse = cIniUtils::getHouseFromString(std::string(chunk));

                if (iHouse > -1) {
                    // Search for a player with this house
                    for (int i = 0; i < MAX_PLAYERS; i++) {
                        if (players[i].getHouse() == iHouse) {
                            playerId = i; // set controller here.. phew
                            break;
                        }
                    }
                }
            }
            else if (iPart == 1) {
                unitType = cIniUtils::getUnitTypeFromString(std::string(chunk));
            }
            else if (iPart == 2) {
                // Homebase is home of that house
                if (strcmp(chunk, "Homebase") == 0) {
                    targetCell = players[playerId].getFocusCell();
                }
                else {
                    // enemy base

                    if (playerId == 0) {
                        // Find corresponding house and get controller
                        for (int i = 0; i < MAX_PLAYERS; i++)
                            if (players[i].getHouse() == iHouse && i != playerId) {
                                targetCell = players[i].getFocusCell();
                                break;
                            }
                    }
                    else {
                        // computer player must find enemy = human
                        targetCell = players[0].getFocusCell();
                    }
                }

            }
            else if (iPart == 3) {
                delayInMinutes = atoi(chunk);
                bool repeat = game.m_allowRepeatingReinforcements && plusDetected;
                int reinforcementMultiplier = 20; // convert minutes to seconds, as D2TM cReinforcement deals with seconds
                // D2TM does not interpret the delay as minutes, as doing so takes a very long time for reinforcements
                // to arrive. So I guess delay is not really 1 minute in game-time in Dune 2.
                // Stefan: 08/04/2022 -> I reduced the multiplier again to 20, as it still takes a very long time;
                // this feels better.
                int delayD2TM = delayInMinutes * reinforcementMultiplier;
                reinforcements->addReinforcement(playerId, unitType, targetCell, delayD2TM, repeat);
                break;
            }

            bClearChunk = true;
        }

        // found the = mark, this means we start chopping now!
        if (linefeed[c] == '=') {
            bSkipped = true;
        }
    }
}

bool INI_Scenario_Section_Structures(int iHumanID, bool bSetUpPlayers, const int *iPl_credits, const int *iPl_house,
                                     const int *iPl_quota,
                                     char *linefeed)  // ORIGINAL DUNE 2 MISSION. (duplicate code?)
{
    if (bSetUpPlayers) {
        INI_Scenario_SetupPlayers(iHumanID, iPl_credits, iPl_house, iPl_quota);
        bSetUpPlayers = false;
    }

    int iPart = -1; /*
                0 = Controller
                1 = Type
                2 = HP
                3 = Cell
                */

    // Skip ID= part. It is just for fun there.
    int iController, iType, iHP, iCell;
    iController = iType = iHP = iCell = -1;

    char chunk[25];
    bool bClearChunk = true;
    bool bSkipped = false;
    int iC = -1;
    bool bGen = false;
    int iIS = -1;

    // check if this is a 'gen'
    if (strstr(linefeed, "GEN") != nullptr) bGen = true;

    for (int c = 0; c < MAX_LINE_LENGTH; c++) {
        // clear chunk
        if (bClearChunk) {
            for (int ic = 0; ic < 25; ic++) {
                chunk[ic] = '\0';
            }

            iC = 0;
            bClearChunk = false;
        }

        // Fill in chunk
        if (iC < 25 && bSkipped && linefeed[c] != ',') {
            chunk[iC] = linefeed[c];
            iC++;
        }


        // , means next part. A ' ' means end
        if (linefeed[c] == ',' || linefeed[c] == '\0') {
            iPart++;

            // this line is not GENXXX
            if (bGen == false) {
                if (iPart == 0) {
                    int iHouse = cIniUtils::getHouseFromString(std::string(chunk));

                    // Search for a player with this house
                    for (int i = 0; i < MAX_PLAYERS; i++) {
                        //char msg[80];
                        //sprintf(msg, "i=%d, ihouse=%d, house=%d", i, iHouse, player[i].house);
                        //logbook(msg);
                        if (players[i].getHouse() == iHouse) {
                            iController = i; // set controller here.. phew
                            break;
                        }
                    }
                    // Quickfix: fremen house in original dune 2, is house 6 (not detectable)
                    // in this game.
                    //if (iHouse == FREMEN)
                    //	iController = 6;

                    if (iController < 0) {
                        logbook("WARNING: Identifying house/controller of structure (typo?)");
                        logbook(chunk);
                    }
                }
                else if (iPart == 1) {
                    iType = cIniUtils::getStructureTypeFromString(std::string(chunk));
                }
                else if (iPart == 3) {
                    iCell = atoi(chunk);

                    break;
                }
            }
            else {
                if (iPart == 0) {
                    int iHouse = cIniUtils::getHouseFromString(std::string(chunk));

                    // Search for a player with this house
                    for (int i = 0; i < MAX_PLAYERS; i++) {
                        if (players[i].getHouse() == iHouse) {
                            iController = i; // set controller here.. phew
                            break;
                        }
                    }
                }
                else if (iPart == 1) {
                    // Figure out the cell shit of this GEN
                    char cCell[5];
                    for (int cc = 0; cc < 5; cc++)
                        cCell[cc] = '\0';

                    int iCC = 0;
                    for (int cc = 3; cc < iIS; cc++) {
                        cCell[iCC] = linefeed[cc];
                        iCC++;
                    }

                    int iGenCell = atoi(cCell);

                    iCell = iGenCell;

                    if (strcmp(chunk, "Wall") == 0) iType = WALL;
                    if (strcmp(chunk, "Concrete") == 0) iType = SLAB1;
                    break;
                }
            }

            bClearChunk = true;
        }

        // found the = mark, this means we start chopping now!
        if (linefeed[c] == '=') {
            bSkipped = true;
            iIS = c;
        }
    }

    if (iController > -1) {
        players[iController].placeStructure(iCell, iType, 100);
    }
    else {
        logbook("WARNING: Identifying house/controller of structure (typo?)");
    }
    return bSetUpPlayers;
}

bool INI_Scenario_Section_Units(int iHumanID, bool bSetUpPlayers, const int *iPl_credits, const int *iPl_house,
                                const int *iPl_quota, const char *linefeed)  // ORIGINAL DUNE 2 MISSION.
{

    // setupPlayers is required, because we do matching based on name of house, hence, once
    // we encounter either a UNITS or STRUCTURES section, the assumption is made that all HOUSE information
    // has been isProcessed.
    if (bSetUpPlayers) {
        INI_Scenario_SetupPlayers(iHumanID, iPl_credits, iPl_house, iPl_quota);
        bSetUpPlayers = false;
    }

    int iPart = -1; /*
                0 = Controller
                1 = Type
                2 = HP
                3 = Cell
                4 = Facing (body)
                5 = Facing (head)
                */

    // Skip ID= part. It is just for fun there.
    int iController, iType, iHP, iCell, iFacingBody, iFacingHead;
    iController = iType = iHP = iCell = iFacingBody = iFacingHead = -1;

    char chunk[25];
    bool bClearChunk = true;
    bool bSkipped = false;
    int iC = -1;

    for (int c = 0; c < MAX_LINE_LENGTH; c++) {

        // clear chunk
        if (bClearChunk) {
            for (int ic = 0; ic < 25; ic++)
                chunk[ic] = '\0';
            iC = 0;
            bClearChunk = false;
        }

        // Fill in chunk
        if (iC < 25 && bSkipped && linefeed[c] != ',') {
            chunk[iC] = linefeed[c];
            iC++;
        }

        // , means next part. A ' ' means end
        if (linefeed[c] == ',' || linefeed[c] == '\0') {
            iPart++;

            if (iPart == INI_UNITS_PART_CONTROLLER) {
                int iHouse = cIniUtils::getHouseFromString(std::string(chunk));

                // Search for a player with this house
                for (int i = 0; i < MAX_PLAYERS; i++) {
                    // this is why we require setUpPlayers... because it matches by house type
                    if (players[i].getHouse() == iHouse) {
                        iController = i; // set controller here.. phew
                        break;
                    }
                }

                if (iController < 0) {
                    logbook(std::format("WARNING: Cannot identify house/controller -> STRING '{}'", chunk));
                }

            }
            else if (iPart == INI_UNITS_PART_TYPE) {
                iType = cIniUtils::getUnitTypeFromString(std::string(chunk));
            }
            else if (iPart == INI_UNITS_PART_HP) {
                // do nothing in part 2 (for now!?)
            }
            else if (iPart == INI_UNITS_PART_CELL) {
                iCell = atoi(chunk);
            }
            else if (iPart == INI_UNITS_PART_FACING_BODY) {
                // we don't seem to care about facings...
                break;
            }

            bClearChunk = true;
        }

        // found the = mark, this means we start chopping now!
        if (linefeed[c] == '=')
            bSkipped = true;
    }

    if (iController > -1) {
        UNIT_CREATE(iCell, iType, iController, true);
    }

    return bSetUpPlayers;
}

void INI_Scenario_SetupPlayers(int iHumanID, const int *iPl_credits, const int *iPl_house, const int *iPl_quota)
{
    logbook("INI: Going to setup players");
    int iCPUId = 1; // index for CPU's, starts at 1 because ID 0 is HUMAN player

    int teamIndexAI = 1;
    bool fremenIsHumanAlly = false;

    for (int playerIndex = 0; playerIndex < MAX_PLAYERS; playerIndex++) { // till 6 , since player 6 itself is sandworm
        int houseForPlayer = iPl_house[playerIndex];
        logbook(std::format("House for id [{}] is [{}] - human id is [{}]", playerIndex, houseForPlayer, iHumanID));
        if (houseForPlayer > -1) {
            int creditsPlayer = iPl_credits[playerIndex];
            int quota = iPl_quota[playerIndex];

            if (playerIndex == iHumanID) {
                logbook(std::format("INI: Setting up human player, credits to [{}], house [{}] and team [{}]", creditsPlayer, houseForPlayer, 0));
                players[HUMAN].setCredits(creditsPlayer);
                players[HUMAN].setHouse(houseForPlayer);
                players[HUMAN].setTeam(0);
                players[HUMAN].setAutoSlabStructures(false);

                // Fremen are always the same CPU index, so check what house the human player is, and depending
                // on that set up FREMEN player team
                players[AI_CPU5].setHouse(FREMEN);
                if (houseForPlayer == ATREIDES) {
                    fremenIsHumanAlly = true;
                }

                assert(drawManager);
                drawManager->missionInit();

                if (quota > 0) {
                    players[HUMAN].setQuota(quota);
                }

            }
            else {
                players[iCPUId].setAutoSlabStructures(true);

                if (quota > 0) {
                    players[iCPUId].setQuota(quota);
                }

                if (houseForPlayer == FREMEN) {
                    // seems like a non-standard Dune 2 mission, this will break
                    assert(false && "No FREMEN supported in INI files yet");
                }

                players[iCPUId].setTeam(teamIndexAI);

                logbook(std::format("INI: Setting up CPU player, credits to [{}], house to [{}] and team [{}]",
                                    creditsPlayer, houseForPlayer, teamIndexAI));

                players[iCPUId].setCredits(creditsPlayer);
                players[iCPUId].setHouse(houseForPlayer);
                iCPUId++;
            }
        }
        else {
            // there is no house set for this player index (from the ini file that is!)
        }
    }

    if (fremenIsHumanAlly) {
        // same team as human
        players[AI_CPU5].setTeam(0);
    }
    else {
        // same team as enemy cpu's
        players[AI_CPU5].setTeam(teamIndexAI);
    }

    players[AI_WORM].setTeam(2); // the WORM player is nobody's ally, ever
}

void INI_LOAD_BRIEFING(int iHouse, int iScenarioFind, int iSectionFind, AbstractMentat *pMentat)
{
    logbook("[BRIEFING] Opening file");

    std::string filename;

    if (iHouse == ATREIDES) filename = "mentata.ini";
    if (iHouse == ORDOS) filename = "mentato.ini";
    if (iHouse == HARKONNEN) filename = "mentath.ini";

    FILE *stream;

    // clear mentat
    pMentat->initSentences();

    auto path = std::string("campaign/briefings/") + filename;
    logbook(path);

    if (game.isDebugMode()) {
        logbook(std::format("Going to find SCEN ID #{} and SectionID {}", iScenarioFind, iSectionFind));
    }

    int iScenario = 0;
    int iSection = 0;
    int iLine = 0; // max 8 lines

    if ((stream = fopen(path.c_str(), "r+t")) != nullptr) {
        char linefeed[MAX_LINE_LENGTH];
        char lineword[30];
        char linesection[30];

        while (!feof(stream)) {
            INI_Sentence(stream, linefeed);

            // Linefeed contains a string of 1 sentence. Whenever the first character is a commentary
            // character (which is "//", ";" or "#"), or an empty line, then skip it
            if (isCommentLine(linefeed)) continue;   // Skip

            INI_Section(linefeed, linesection);

            if (linesection[0] != '\0' && strlen(linesection) > 1) {
                // until we found the right sections/parts, keep searching
                iSection = cIniUtils::getSectionType(linesection, iSection);
            }

            if (iSection == INI_SCEN) {
                INI_Word(linefeed, lineword);
                int wordtype = INI_WordType(lineword, iSection);

                if (wordtype == WORD_NUMBER) {
                    iScenario = INI_WordValueINT(linefeed);
                    continue;
                }
            }

            if (iScenario == iScenarioFind) {
                if (iSection == iSectionFind) {
                    INI_Word(linefeed, lineword);

                    int wordtype = INI_WordType(lineword, iSection);

                    if (wordtype == WORD_REGIONTEXT) {
                        char cHouseText[256];
                        INI_WordValueSENTENCE(linefeed, cHouseText);

                        // this is not a comment, add this....
                        pMentat->setSentence(iLine, cHouseText);
                        iLine++;

                        if (iLine > 9)
                            break;
                    }
                }
            }
        }

        fclose(stream);
    }


    logbook("[BRIEFING] File opened");
}


// Game.ini loader
void INI_Install_Game(std::string filename)
{
    logbook("[GameRules] Opening file");

    FILE *stream;
    int section = INI_GAME;
    int wordtype = WORD_NONE;
    int id = -1;

    logbook(std::format("Opening game settings from : {}", filename));

    if ((stream = fopen(filename.c_str(), "r+t")) != nullptr) {
        char linefeed[MAX_LINE_LENGTH];
        char lineword[30];
        char linesection[30];

        // infinite loop baby
        while (!feof(stream)) {
            INI_Sentence(stream, linefeed);

            // Linefeed contains a string of 1 sentence. Whenever the first character is a commentary
            // character (which is "//", ";" or "#"), or an empty line, then skip it
            if (isCommentLine(linefeed))
                continue;   // Skip

            wordtype = WORD_NONE;

            // Every line is checked for a new section.
            INI_Section(linefeed, linesection);

            if (linesection[0] != '\0' && strlen(linesection) > 1) {
                int last = section;

                // determine section
                section = GAME_INI_SectionType(linesection, section);

                // When we changed section and its a section with adding ID's. We should reset
                // the ID var
                if (last != section) {
                    id = -1;

                    // Show in log file we entered a new section
                    if (section == INI_UNITS) logbook("[GAME.INI] -> [UNITS]");
                    if (section == INI_STRUCTURES) logbook("[GAME.INI] -> [STRUCTURES]");
                }

                // New unit type
                if (section == INI_UNITS) {
                    // check if we found a new [UNIT part!
                    if (strstr(linefeed, "[UNIT:") != nullptr) {
                        // Get the name of the unit:
                        // [UNIT: <NAME>]
                        // 1234567890123...]
                        char name_unit[35];

                        for (int nu = 0; nu < 35; nu++)
                            name_unit[nu] = '\0';

                        int c = 7, uc = 0;
                        while (c < (MAX_LINE_LENGTH - 1)) {
                            if (linefeed[c] != ' ' &&
                                    linefeed[c] != ']') { // skip close bracket
                                name_unit[uc] = linefeed[c];
                                uc++;
                                c++;
                            }
                            else {
                                break; // get out
                            }
                        }

                        id = cIniUtils::getUnitTypeFromString(std::string(name_unit));
                        if (id >= MAX_UNITTYPES) id--;

                    } // found a new unit type
                }

                // New structure type
                if (section == INI_STRUCTURES) {
                    // check if we found a new [STRUCTURE: part!
                    if (strstr(linefeed, "[STRUCTURE:") != nullptr) {
                        // Get the name of the unit:
                        // [STRUCTURE: <NAME>]
                        // 123456789012345678]
                        char name_structure[45];

                        for (int nu = 0; nu < 45; nu++)
                            name_structure[nu] = '\0';

                        int c = 12, uc = 0;
                        while (c < (MAX_LINE_LENGTH - 1)) {
                            if (linefeed[c] != ' ' &&
                                    linefeed[c] != ']') { // skip close bracket
                                name_structure[uc] = linefeed[c];
                                uc++;
                                c++;
                            }
                            else
                                break; // get out
                        }

                        id = cIniUtils::getStructureType(name_structure);
                        if (id >= MAX_STRUCTURETYPES) id--;

                    } // found a new structure type
                }

                continue; // next line
            }

            // Check word only when in a section
            if (section != INI_GAME) {
                INI_Word(linefeed, lineword);
                wordtype = INI_WordType(lineword, section);

                /**** [UNITS] ****/
                // Valid ID
                if (section == INI_UNITS && id > -1) {
                    // Unit properties
                    s_UnitInfo &unitInfo = sUnitInfo[id];

                    if (wordtype == WORD_HITPOINTS) unitInfo.hp = INI_WordValueINT(linefeed);
                    if (wordtype == WORD_APPETITE) unitInfo.appetite = INI_WordValueINT(linefeed);

                    if (wordtype == WORD_COST) unitInfo.cost = INI_WordValueINT(linefeed);

                    if (wordtype == WORD_MOVESPEED) unitInfo.speed = INI_WordValueINT(linefeed);
                    if (wordtype == WORD_TURNSPEED) unitInfo.turnspeed = INI_WordValueINT(linefeed);
                    if (wordtype == WORD_ATTACKFREQ) unitInfo.attack_frequency = INI_WordValueINT(linefeed);
                    if (wordtype == WORD_NEXTATTACKFREQ) unitInfo.next_attack_frequency = INI_WordValueINT(linefeed);

                    if (wordtype == WORD_SIGHT) unitInfo.sight = INI_WordValueINT(linefeed);

                    if (wordtype == WORD_RANGE) unitInfo.range = INI_WordValueINT(linefeed);
                    if (wordtype == WORD_BUILDTIME) unitInfo.buildTime = INI_WordValueINT(linefeed);

                    // Unit description
                    if (wordtype == WORD_DESCRIPTION) {
                        char n[256];
                        INI_WordValueCHAR(linefeed, n);
                        strncpy(unitInfo.name, n, sizeof(unitInfo.name) - 1);
                        unitInfo.name[sizeof(unitInfo.name) - 1] = '\0';
                    }

                    // Booleans
                    if (wordtype == WORD_FIRETWICE) unitInfo.fireTwice = INI_WordValueBOOL(linefeed);
                    if (wordtype == WORD_ISINFANTRY) unitInfo.infantry = INI_WordValueBOOL(linefeed);
                    if (wordtype == WORD_ISSQUISHABLE) unitInfo.canBeSquished = INI_WordValueBOOL(linefeed);
                    if (wordtype == WORD_CANSQUISH) unitInfo.canBeSquished = INI_WordValueBOOL(linefeed);
                    if (wordtype == WORD_FREEROAM) unitInfo.free_roam = INI_WordValueBOOL(linefeed);
                    if (wordtype == WORD_ISAIRBORN) unitInfo.airborn = INI_WordValueBOOL(linefeed);

                    // Harvester specific properties.
                    if (wordtype == WORD_HARVESTLIMIT) unitInfo.credit_capacity = INI_WordValueINT(linefeed);
                    if (wordtype == WORD_HARVESTSPEED) unitInfo.harvesting_speed = INI_WordValueINT(linefeed);
                    if (wordtype == WORD_HARVESTAMOUNT) unitInfo.harvesting_amount = INI_WordValueINT(linefeed);

                    if (wordtype == WORD_PRODUCER) {
                        std::string producerString = INI_WordValueString(linefeed);
                        // determine structure type from that
                        int type = cIniUtils::getStructureType(producerString);
                        unitInfo.structureTypeItLeavesFrom = type;
                    }
                }
            }

            // Structure w0h00
            if (section == INI_STRUCTURES && id > -1) {
                if (wordtype == WORD_HITPOINTS) {
                    sStructureInfo[id].hp = INI_WordValueINT(linefeed);
                }
                if (wordtype == WORD_FIXHP) sStructureInfo[id].fixhp = INI_WordValueINT(linefeed);

                if (wordtype == WORD_POWERDRAIN) sStructureInfo[id].power_drain = INI_WordValueINT(linefeed);
                if (wordtype == WORD_POWERGIVE) sStructureInfo[id].power_give = INI_WordValueINT(linefeed);

                if (wordtype == WORD_COST) sStructureInfo[id].cost = INI_WordValueINT(linefeed);
                if (wordtype == WORD_BUILDTIME) sStructureInfo[id].buildTime = INI_WordValueINT(linefeed);
            }
        } // while

        fclose(stream);
    } // if

    logbook("[GAME.INI] Done");
}
