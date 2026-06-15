#include "cHousesInfo.h"
#include "include/definitions.h"
#include "utils/cIniUtils.h"
#include "utils/Log.h"

namespace {
    struct HouseEntry { int house; const char* section; };
    constexpr HouseEntry kHouses[] = {
        {GENERALHOUSE, "HOUSE: GENERAL"},
        {HARKONNEN,    "HOUSE: HARKONNEN"},
        {ATREIDES,     "HOUSE: ATREIDES"},
        {ORDOS,        "HOUSE: ORDOS"},
        {SARDAUKAR,    "HOUSE: SARDAUKAR"},
        {FREMEN,       "HOUSE: FREMEN"},
        {MERCENARY,    "HOUSE: MERCENARY"},
        {CORRINO,      "HOUSE: CORRINO"},
    };
}

cHousesInfo::cHousesInfo()
{
    m_houseInfo[GENERALHOUSE].minimap_color = Color(128,128,128);
    m_houseInfo[GENERALHOUSE].colors = {{{194, 194, 218}, {157, 157, 186}, {125, 125, 153},
                                    {93, 93, 121}, { 64, 64, 89}, { 40, 40, 60}, { 20, 20, 40}}};

    // Harkonnen
    m_houseInfo[HARKONNEN].minimap_color = Color(255,0,0);
    m_houseInfo[HARKONNEN].colors = {{{214, 0, 0}, {182, 0, 0}, {153, 0, 0},
                                    {125, 0, 0}, { 89, 0, 0}, { 60, 0, 0}, { 32, 0, 0}}};

    // Atreides
    m_houseInfo[ATREIDES].minimap_color = Color(0,0,255);
    m_houseInfo[ATREIDES].colors = {{{80, 121, 214}, {52, 85, 182}, {40, 60, 153},
                                    {24, 32, 125}, { 8, 12, 89}, { 4, 4, 60}, { 0, 0, 32}}};

    // Ordos
    m_houseInfo[ORDOS].minimap_color = Color(0,255,0);
    m_houseInfo[ORDOS].colors = {{{76, 214, 76}, {56, 182, 56}, {36, 153, 36},
                                    {24, 125, 24}, {12, 89, 12}, { 4, 60, 4}, { 0, 32, 0}}};

    // Mercenary
    m_houseInfo[MERCENARY].minimap_color = Color(214, 121, 16);
    m_houseInfo[MERCENARY].colors = {{{214, 121, 16}, {182, 97, 8}, {153, 76, 4},
                                    {121, 60, 4}, {93, 40, 0}, { 60, 24, 0}, { 32, 12, 0}}};

    // Sardaukar
    m_houseInfo[SARDAUKAR].minimap_color = Color(255,0,255);
    m_houseInfo[SARDAUKAR].colors = {{{242, 105, 242}, {206, 72, 206}, {170, 48, 170},
                                    {137, 24, 137}, {101, 12, 101}, { 64, 0, 64}, { 32, 0, 32}}};

    // Fremen
    m_houseInfo[FREMEN].minimap_color = Color(194,125,60);
    m_houseInfo[FREMEN].colors = {{{214, 149, 0}, {182, 125, 0}, {153, 105, 0},
                                 {125, 80, 0}, {89, 56, 0}, { 60, 36, 0}, { 32, 16, 0}}};

    // CORRINO
    m_houseInfo[CORRINO].minimap_color = Color(192,192,192);
    m_houseInfo[CORRINO].colors = {{{226, 182, 149}, {202, 161, 133}, {178, 141, 117},
                                    {153, 121, 101}, {129, 101, 85}, { 105, 80, 68}, { 80, 60, 52}}};    
}


void cHousesInfo::installHouses(std::shared_ptr<cIniFile> gameCfg)
{
    for (auto& [house, sectionName] : kHouses) {
        if (!gameCfg->hasSection(sectionName)) continue;
        const cSection& section = gameCfg->getSection(sectionName);
        if (section.hasValue("MINIMAPCOLOR")) {
            m_houseInfo[house].minimap_color = cIniUtils::colorFromString(section.getStringValue("MINIMAPCOLOR"));
        }
        if (section.hasValue("TEAMCOLOR_0")) {
            for (size_t i = 0; i < m_houseInfo[house].colors.size(); i++) {
                std::string key = "TEAMCOLOR_" + std::to_string(i);
                if (section.hasValue(key)) {
                    m_houseInfo[house].colors[i] = cIniUtils::houseColorFromString(section.getStringValue(key));
                } else {
                    Logger::warn(COMP_GAMERULES, "cHousesInfo::installHouses", "Missing {} in section [{}], using default", key, sectionName);
                }
            }
        }
    }
}

