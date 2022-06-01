#include "cHousesInfo.h"
#include "include/definitions.h"
#include "utils/common.h"

void cHousesInfo::INSTALL_HOUSES(std::shared_ptr<cIniFile> gameCfg) {
    const cSection &section = gameCfg->getSection(SECTION_HOUSES);

    // General / Default / No House
    sHouseInfo[GENERALHOUSE].swap_color = section.getInt("GENERAL_SWAPCOLOR");
    sHouseInfo[GENERALHOUSE].minimap_color = makeColFromString(section.getStringValue("GENERAL_MINIMAPCOLOR"));

    // Harkonnen
    sHouseInfo[HARKONNEN].swap_color = section.getInt("HARKONNEN_SWAPCOLOR");
    sHouseInfo[HARKONNEN].minimap_color = makeColFromString(section.getStringValue("HARKONNEN_MINIMAPCOLOR"));

    // Atreides
    sHouseInfo[ATREIDES].swap_color = section.getInt("ATREIDES_SWAPCOLOR");
    sHouseInfo[ATREIDES].minimap_color = makeColFromString(section.getStringValue("ATREIDES_MINIMAPCOLOR"));

    // Ordos
    sHouseInfo[ORDOS].swap_color = section.getInt("ORDOS_SWAPCOLOR");
    sHouseInfo[ORDOS].minimap_color = makeColFromString(section.getStringValue("ORDOS_MINIMAPCOLOR"));

    // Mercenary
    sHouseInfo[MERCENARY].swap_color = section.getInt("MERCENARY_SWAPCOLOR");
    sHouseInfo[MERCENARY].minimap_color = makeColFromString(section.getStringValue("MERCENARY_MINIMAPCOLOR"));

    // Sardaukar
    sHouseInfo[SARDAUKAR].swap_color = section.getInt("SARDAUKAR_SWAPCOLOR");
    sHouseInfo[SARDAUKAR].minimap_color = makeColFromString(section.getStringValue("SARDAUKAR_MINIMAPCOLOR"));

    // Fremen
    sHouseInfo[FREMEN].swap_color = section.getInt("FREMEN_SWAPCOLOR");
    sHouseInfo[FREMEN].minimap_color = makeColFromString(section.getStringValue("FREMEN_MINIMAPCOLOR"));

    // GREY
    // ???
    sHouseInfo[CORRINO].swap_color = section.getInt("CORRINO_SWAPCOLOR");
    sHouseInfo[CORRINO].minimap_color = makeColFromString(section.getStringValue("CORRINO_MINIMAPCOLOR"));
}
