#include "cHousesInfo.h"
#include "include/definitions.h"

#include <algorithm>

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
    const cSection &section = gameCfg->getSection(SECTION_HOUSES);

    // General / Default / No House
    m_houseInfo[GENERALHOUSE].minimap_color = makeColFromString(section.getStringValue("GENERAL_MINIMAPCOLOR"));

    // Harkonnen
    m_houseInfo[HARKONNEN].minimap_color = makeColFromString(section.getStringValue("HARKONNEN_MINIMAPCOLOR"));

    // Atreides
    m_houseInfo[ATREIDES].minimap_color = makeColFromString(section.getStringValue("ATREIDES_MINIMAPCOLOR"));

    // Ordos
    m_houseInfo[ORDOS].minimap_color = makeColFromString(section.getStringValue("ORDOS_MINIMAPCOLOR"));

    // Mercenary
    m_houseInfo[MERCENARY].minimap_color = makeColFromString(section.getStringValue("MERCENARY_MINIMAPCOLOR"));

    // Sardaukar
    m_houseInfo[SARDAUKAR].minimap_color = makeColFromString(section.getStringValue("SARDAUKAR_MINIMAPCOLOR"));

    // Fremen
    m_houseInfo[FREMEN].minimap_color = makeColFromString(section.getStringValue("FREMEN_MINIMAPCOLOR"));

    // CORRINO
    m_houseInfo[CORRINO].minimap_color = makeColFromString(section.getStringValue("CORRINO_MINIMAPCOLOR"));
}

Color cHousesInfo::makeColFromString(std::string colorStr)
{
    int r = 0, g = 0, b = 0;
    char comma; // Pour consommer les virgules

    std::stringstream ss(colorStr);
    ss >> r >> comma >> g >> comma >> b;
    //std::cout << r << ":"<<g<<":"<<b<<std::endl;
    return Color{(Uint8)r,(Uint8)g,(Uint8)b,255};
}