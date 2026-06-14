#include "cHousesInfo.h"
#include "include/definitions.h"
#include "utils/cIniUtils.h"

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
    if (section.hasValue("GENERAL_MINIMAPCOLOR")) {
        m_houseInfo[GENERALHOUSE].minimap_color = cIniUtils::colorFromString(section.getStringValue("GENERAL_MINIMAPCOLOR"));
    }
    if (section.hasValue("GENERAL_COLORS_0")) { //we assume if colors 0 is set, the rest is set too, and in the correct format. No need to check each one.
        m_houseInfo[GENERALHOUSE].colors[0] = cIniUtils::houseColorFromString(section.getStringValue("GENERAL_COLORS_0"));
        m_houseInfo[GENERALHOUSE].colors[1] = cIniUtils::houseColorFromString(section.getStringValue("GENERAL_COLORS_1"));
        m_houseInfo[GENERALHOUSE].colors[2] = cIniUtils::houseColorFromString(section.getStringValue("GENERAL_COLORS_2"));
        m_houseInfo[GENERALHOUSE].colors[3] = cIniUtils::houseColorFromString(section.getStringValue("GENERAL_COLORS_3"));
        m_houseInfo[GENERALHOUSE].colors[4] = cIniUtils::houseColorFromString(section.getStringValue("GENERAL_COLORS_4"));
        m_houseInfo[GENERALHOUSE].colors[5] = cIniUtils::houseColorFromString(section.getStringValue("GENERAL_COLORS_5"));
        m_houseInfo[GENERALHOUSE].colors[6] = cIniUtils::houseColorFromString(section.getStringValue("GENERAL_COLORS_6"));
    }

    // Harkonnen
    if (section.hasValue("HARKONNEN_MINIMAPCOLOR")) {
        m_houseInfo[HARKONNEN].minimap_color = cIniUtils::colorFromString(section.getStringValue("HARKONNEN_MINIMAPCOLOR"));
    }
    if (section.hasValue("HARKONNEN_COLORS_0")) { //we assume if colors 0 is set, the rest is set too, and in the correct format. No need to check each one.
        m_houseInfo[HARKONNEN].colors[0] = cIniUtils::houseColorFromString(section.getStringValue("HARKONNEN_COLORS_0"));
        m_houseInfo[HARKONNEN].colors[1] = cIniUtils::houseColorFromString(section.getStringValue("HARKONNEN_COLORS_1"));
        m_houseInfo[HARKONNEN].colors[2] = cIniUtils::houseColorFromString(section.getStringValue("HARKONNEN_COLORS_2"));
        m_houseInfo[HARKONNEN].colors[3] = cIniUtils::houseColorFromString(section.getStringValue("HARKONNEN_COLORS_3"));
        m_houseInfo[HARKONNEN].colors[4] = cIniUtils::houseColorFromString(section.getStringValue("HARKONNEN_COLORS_4"));
        m_houseInfo[HARKONNEN].colors[5] = cIniUtils::houseColorFromString(section.getStringValue("HARKONNEN_COLORS_5"));
        m_houseInfo[HARKONNEN].colors[6] = cIniUtils::houseColorFromString(section.getStringValue("HARKONNEN_COLORS_6"));
    }

    // Atreides
    if (section.hasValue("ATREIDES_MINIMAPCOLOR")) {
        m_houseInfo[ATREIDES].minimap_color = cIniUtils::colorFromString(section.getStringValue("ATREIDES_MINIMAPCOLOR"));
    }
    if (section.hasValue("ATREIDES_COLORS_0")) { //we assume if colors 0 is set, the rest is set too, and in the correct format. No need to check each one.
        m_houseInfo[ATREIDES].colors[0] = cIniUtils::houseColorFromString(section.getStringValue("ATREIDES_COLORS_0"));
        m_houseInfo[ATREIDES].colors[1] = cIniUtils::houseColorFromString(section.getStringValue("ATREIDES_COLORS_1"));
        m_houseInfo[ATREIDES].colors[2] = cIniUtils::houseColorFromString(section.getStringValue("ATREIDES_COLORS_2"));
        m_houseInfo[ATREIDES].colors[3] = cIniUtils::houseColorFromString(section.getStringValue("ATREIDES_COLORS_3"));
        m_houseInfo[ATREIDES].colors[4] = cIniUtils::houseColorFromString(section.getStringValue("ATREIDES_COLORS_4"));
        m_houseInfo[ATREIDES].colors[5] = cIniUtils::houseColorFromString(section.getStringValue("ATREIDES_COLORS_5"));
        m_houseInfo[ATREIDES].colors[6] = cIniUtils::houseColorFromString(section.getStringValue("ATREIDES_COLORS_6"));
    }

    // Ordos
    if (section.hasValue("ORDOS_MINIMAPCOLOR")) {
        m_houseInfo[ORDOS].minimap_color = cIniUtils::colorFromString(section.getStringValue("ORDOS_MINIMAPCOLOR"));
    }
    if (section.hasValue("ORDOS_COLORS_0")) { //we assume if colors 0 is set, the rest is set too, and in the correct format. No need to check each one.
        m_houseInfo[ORDOS].colors[0] = cIniUtils::houseColorFromString(section.getStringValue("ORDOS_COLORS_0"));
        m_houseInfo[ORDOS].colors[1] = cIniUtils::houseColorFromString(section.getStringValue("ORDOS_COLORS_1"));
        m_houseInfo[ORDOS].colors[2] = cIniUtils::houseColorFromString(section.getStringValue("ORDOS_COLORS_2"));
        m_houseInfo[ORDOS].colors[3] = cIniUtils::houseColorFromString(section.getStringValue("ORDOS_COLORS_3"));
        m_houseInfo[ORDOS].colors[4] = cIniUtils::houseColorFromString(section.getStringValue("ORDOS_COLORS_4"));
        m_houseInfo[ORDOS].colors[5] = cIniUtils::houseColorFromString(section.getStringValue("ORDOS_COLORS_5"));
        m_houseInfo[ORDOS].colors[6] = cIniUtils::houseColorFromString(section.getStringValue("ORDOS_COLORS_6"));
    }

    // Mercenary
    if (section.hasValue("MERCENARY_MINIMAPCOLOR")) {    
        m_houseInfo[MERCENARY].minimap_color = cIniUtils::colorFromString(section.getStringValue("MERCENARY_MINIMAPCOLOR"));
    }
    if (section.hasValue("MERCENARY_COLORS_0")) { //we assume if colors 0 is set, the rest is set too, and in the correct format. No need to check each one.
        m_houseInfo[MERCENARY].colors[0] = cIniUtils::houseColorFromString(section.getStringValue("MERCENARY_COLORS_0"));
        m_houseInfo[MERCENARY].colors[1] = cIniUtils::houseColorFromString(section.getStringValue("MERCENARY_COLORS_1"));
        m_houseInfo[MERCENARY].colors[2] = cIniUtils::houseColorFromString(section.getStringValue("MERCENARY_COLORS_2"));
        m_houseInfo[MERCENARY].colors[3] = cIniUtils::houseColorFromString(section.getStringValue("MERCENARY_COLORS_3"));
        m_houseInfo[MERCENARY].colors[4] = cIniUtils::houseColorFromString(section.getStringValue("MERCENARY_COLORS_4"));
        m_houseInfo[MERCENARY].colors[5] = cIniUtils::houseColorFromString(section.getStringValue("MERCENARY_COLORS_5"));
        m_houseInfo[MERCENARY].colors[6] = cIniUtils::houseColorFromString(section.getStringValue("MERCENARY_COLORS_6"));
    }

    // Sardaukar
    if (section.hasValue("SARDAUKAR_MINIMAPCOLOR")) {
        m_houseInfo[SARDAUKAR].minimap_color = cIniUtils::colorFromString(section.getStringValue("SARDAUKAR_MINIMAPCOLOR"));
    }
    if (section.hasValue("SARDAUKAR_COLORS_0")) { //we assume if colors 0 is set, the rest is set too, and in the correct format. No need to check each one.
        m_houseInfo[SARDAUKAR].colors[0] = cIniUtils::houseColorFromString(section.getStringValue("SARDAUKAR_COLORS_0"));
        m_houseInfo[SARDAUKAR].colors[1] = cIniUtils::houseColorFromString(section.getStringValue("SARDAUKAR_COLORS_1"));
        m_houseInfo[SARDAUKAR].colors[2] = cIniUtils::houseColorFromString(section.getStringValue("SARDAUKAR_COLORS_2"));
        m_houseInfo[SARDAUKAR].colors[3] = cIniUtils::houseColorFromString(section.getStringValue("SARDAUKAR_COLORS_3"));
        m_houseInfo[SARDAUKAR].colors[4] = cIniUtils::houseColorFromString(section.getStringValue("SARDAUKAR_COLORS_4"));
        m_houseInfo[SARDAUKAR].colors[5] = cIniUtils::houseColorFromString(section.getStringValue("SARDAUKAR_COLORS_5"));
        m_houseInfo[SARDAUKAR].colors[6] = cIniUtils::houseColorFromString(section.getStringValue("SARDAUKAR_COLORS_6"));
    }

    // Fremen
    if (section.hasValue("FREMEN_MINIMAPCOLOR")) {
        m_houseInfo[FREMEN].minimap_color = cIniUtils::colorFromString(section.getStringValue("FREMEN_MINIMAPCOLOR"));
    }
    if (section.hasValue("FREMEN_COLORS_0")) { //we assume if colors 0 is set, the rest is set too, and in the correct format. No need to check each one.
        m_houseInfo[FREMEN].colors[0] = cIniUtils::houseColorFromString(section.getStringValue("FREMEN_COLORS_0"));
        m_houseInfo[FREMEN].colors[1] = cIniUtils::houseColorFromString(section.getStringValue("FREMEN_COLORS_1"));
        m_houseInfo[FREMEN].colors[2] = cIniUtils::houseColorFromString(section.getStringValue("FREMEN_COLORS_2"));
        m_houseInfo[FREMEN].colors[3] = cIniUtils::houseColorFromString(section.getStringValue("FREMEN_COLORS_3"));
        m_houseInfo[FREMEN].colors[4] = cIniUtils::houseColorFromString(section.getStringValue("FREMEN_COLORS_4"));
        m_houseInfo[FREMEN].colors[5] = cIniUtils::houseColorFromString(section.getStringValue("FREMEN_COLORS_5"));
        m_houseInfo[FREMEN].colors[6] = cIniUtils::houseColorFromString(section.getStringValue("FREMEN_COLORS_6"));
    }

    // CORRINO
    if (section.hasValue("CORRINO_MINIMAPCOLOR")) {
        m_houseInfo[CORRINO].minimap_color = cIniUtils::colorFromString(section.getStringValue("CORRINO_MINIMAPCOLOR"));
    }
    if (section.hasValue("CORRINO_COLORS_0")) { //we assume if colors 0 is set, the rest is set too, and in the correct format. No need to check each one.
        m_houseInfo[CORRINO].colors[0] = cIniUtils::houseColorFromString(section.getStringValue("CORRINO_COLORS_0"));
        m_houseInfo[CORRINO].colors[1] = cIniUtils::houseColorFromString(section.getStringValue("CORRINO_COLORS_1"));
        m_houseInfo[CORRINO].colors[2] = cIniUtils::houseColorFromString(section.getStringValue("CORRINO_COLORS_2"));
        m_houseInfo[CORRINO].colors[3] = cIniUtils::houseColorFromString(section.getStringValue("CORRINO_COLORS_3"));
        m_houseInfo[CORRINO].colors[4] = cIniUtils::houseColorFromString(section.getStringValue("CORRINO_COLORS_4"));
        m_houseInfo[CORRINO].colors[5] = cIniUtils::houseColorFromString(section.getStringValue("CORRINO_COLORS_5"));
        m_houseInfo[CORRINO].colors[6] = cIniUtils::houseColorFromString(section.getStringValue("CORRINO_COLORS_6"));
    }
}

