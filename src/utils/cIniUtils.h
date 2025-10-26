#pragma once

#include <string>
#include <unordered_map>

class cIniUtils {
public:
    static int getHouseFromString(const std::string& chunk);
    static int getUnitTypeFromString(const std::string& chunk);
    static std::string getSceneFileToScene(const std::string& scenefile);
    static int getStructureType(const std::string& structureName);
    static std::string getScenarioFileName(int iHouse, int iRegion);
    static int getSectionType(const std::string& section, int last);
    static int getStructureTypeFromString(const std::string& structureStr); 

    static const std::unordered_map<std::string, int> sectionMap;
    static const std::unordered_map<std::string, int> houseMap;
    static const std::unordered_map<std::string, int> structureMap;
    static const std::unordered_map<std::string, int> sectionTypeMap;
    static const std::unordered_map<std::string, std::string> sceneFileMap;
    static const std::unordered_map<std::string, int> structureNameMap;
    static const std::unordered_map<std::string, int> unitNameMap;
    static const std::unordered_map<int, char> houseLetterMap;

    static bool caseInsCompare(const std::string& s1, const std::string& s2);
};