#pragma once

#include <stdexcept>
#include <string>
#include <sstream>
#include <unordered_map>
#include "utils/Color.hpp"
#include "utils/HouseColors.h"

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
    // Returns the bullet type ID for the given bullet name string.
    static int getBulletTypeFromString(const std::string& chunk);
    // Compares two strings case-insensitively. Returns true if they are equal.
    static bool caseInsCompare(const std::string& s1, const std::string& s2);

    static Color colorFromString(const std::string& colorStr) {
        auto rgb = parseRGB(colorStr);
        return Color{rgb.r, rgb.g, rgb.b, 255};
    }

    static HouseColor houseColorFromString(const std::string& colorStr) {
        return parseRGB(colorStr);
    }

    // unordered_map IDs to their corresponding objects.
private:

    static Uint8 parseNumber(const std::string& token, const std::string& str) {
        auto start = token.find_first_not_of(" \t");
        auto end   = token.find_last_not_of(" \t");
        if (start == std::string::npos) {
            throw std::invalid_argument("Empty color component in: \"" + str + "\"");
        }
        std::string trimmed = token.substr(start, end - start + 1);
        int value;
        try {
            value = std::stoi(trimmed);
        } catch (const std::exception&) {
            throw std::invalid_argument("Invalid value provided '" + trimmed + "' in: \"" + str + "\", expected 0-255,0-255,0-255");
        }
        if (value < 0 || value > 255) {
            throw std::out_of_range("Color component " + trimmed + " is out of range [0,255] in: \"" + str + "\"");
        }
        return static_cast<Uint8>(value);
    }

    static HouseColor parseRGB(const std::string& colorStr) {
        std::istringstream ss(colorStr);
        std::string r, g, b;
        std::getline(ss, r, ',');
        std::getline(ss, g, ',');
        std::getline(ss, b, ',');
        return HouseColor{
            parseNumber(r, colorStr),
            parseNumber(g, colorStr),
            parseNumber(b, colorStr)
        };
    }

    static const std::unordered_map<std::string, int> sectionMap;
    static const std::unordered_map<std::string, int> houseMap;
    static const std::unordered_map<std::string, int> structureMap;
    static const std::unordered_map<std::string, int> sectionTypeMap;
    static const std::unordered_map<std::string, std::string> sceneFileMap;
    static const std::unordered_map<std::string, int> structureNameMap;
    static const std::unordered_map<std::string, int> unitNameMap;
    static const std::unordered_map<std::string, int> bulletNameMap;
    static const std::unordered_map<int, char> houseLetterMap;
};