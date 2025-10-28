#pragma once

#include <string>
#include <unordered_map>

class cIniUtils {
public:
    // Returns the house ID corresponding to the given house name string.
    static int getHouseFromString(const std::string& chunk);
    // Returns the unit type ID corresponding to the given unit name string.
    static int getUnitTypeFromString(const std::string& chunk);
    // Maps a scenario file name to its corresponding scene name.
    static std::string getSceneFileToScene(const std::string& scenefile);
    // Returns the structure type ID for the given structure name.
    static int getStructureType(const std::string& structureName);
    // Returns the scenario file name for the specified house and region.
    static std::string getScenarioFileName(int iHouse, int iRegion);
    // Returns the section type ID for the given section name or last if not found.
    static int getSectionType(const std::string& section, int last);
    // Returns the structure type ID for the given structure name string (alternative to getStructureType).
    static int getStructureTypeFromString(const std::string& structureStr); 
    // Compares two strings case-insensitively. Returns true if they are equal.
    static bool caseInsCompare(const std::string& s1, const std::string& s2);
    // unordered_map IDs to their corresponding objects.
    static const std::unordered_map<std::string, int> sectionMap;
    static const std::unordered_map<std::string, int> houseMap;
    static const std::unordered_map<std::string, int> structureMap;
    static const std::unordered_map<std::string, int> sectionTypeMap;
    static const std::unordered_map<std::string, std::string> sceneFileMap;
    static const std::unordered_map<std::string, int> structureNameMap;
    static const std::unordered_map<std::string, int> unitNameMap;
    static const std::unordered_map<int, char> houseLetterMap;
};