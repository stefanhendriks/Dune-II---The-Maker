#include "cHousesInfo.h"
#include "include/definitions.h"
#include "utils/common.h"

void cHousesInfo::INSTALL_HOUSES(std::shared_ptr<cIniFile> gameCfg) {
    const cSection &section = gameCfg->getSection(SECTION_HOUSES);

    // General / Default / No House
    m_houseInfo[GENERALHOUSE].swap_color = section.getInt("GENERAL_SWAPCOLOR");
    m_houseInfo[GENERALHOUSE].minimap_color = makeColFromString(section.getStringValue("GENERAL_MINIMAPCOLOR"));

    // Harkonnen
    m_houseInfo[HARKONNEN].swap_color = section.getInt("HARKONNEN_SWAPCOLOR");
    m_houseInfo[HARKONNEN].minimap_color = makeColFromString(section.getStringValue("HARKONNEN_MINIMAPCOLOR"));

    // Atreides
    m_houseInfo[ATREIDES].swap_color = section.getInt("ATREIDES_SWAPCOLOR");
    m_houseInfo[ATREIDES].minimap_color = makeColFromString(section.getStringValue("ATREIDES_MINIMAPCOLOR"));

    // Ordos
    m_houseInfo[ORDOS].swap_color = section.getInt("ORDOS_SWAPCOLOR");
    m_houseInfo[ORDOS].minimap_color = makeColFromString(section.getStringValue("ORDOS_MINIMAPCOLOR"));

    // Mercenary
    m_houseInfo[MERCENARY].swap_color = section.getInt("MERCENARY_SWAPCOLOR");
    m_houseInfo[MERCENARY].minimap_color = makeColFromString(section.getStringValue("MERCENARY_MINIMAPCOLOR"));

    // Sardaukar
    m_houseInfo[SARDAUKAR].swap_color = section.getInt("SARDAUKAR_SWAPCOLOR");
    m_houseInfo[SARDAUKAR].minimap_color = makeColFromString(section.getStringValue("SARDAUKAR_MINIMAPCOLOR"));

    // Fremen
    m_houseInfo[FREMEN].swap_color = section.getInt("FREMEN_SWAPCOLOR");
    m_houseInfo[FREMEN].minimap_color = makeColFromString(section.getStringValue("FREMEN_MINIMAPCOLOR"));

    // GREY
    // ???
    m_houseInfo[CORRINO].swap_color = section.getInt("CORRINO_SWAPCOLOR");
    m_houseInfo[CORRINO].minimap_color = makeColFromString(section.getStringValue("CORRINO_MINIMAPCOLOR"));
}
