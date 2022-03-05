#include "cHousesInfo.h"
#include "include/definitions.h"
#include "utils/common.h"

void cHousesInfo::INSTALL_HOUSES(std::shared_ptr<cIniFile> gamesCfg) {
    // General / Default / No House
    sHouseInfo[GENERALHOUSE].swap_color = gamesCfg->getInt("HOUSES","GENERAL_SWAPCOLOR");
    sHouseInfo[GENERALHOUSE].minimap_color = makeColFromString(gamesCfg->getStr("HOUSES","GENERAL_MINIMAPCOLOR"));

    // Harkonnen
    sHouseInfo[HARKONNEN].swap_color = gamesCfg->getInt("HOUSES","HARKONNEN_SWAPCOLOR");
    sHouseInfo[HARKONNEN].minimap_color = makeColFromString(gamesCfg->getStr("HOUSES","HARKONNEN_MINIMAPCOLOR"));

    // Atreides
    sHouseInfo[ATREIDES].swap_color = gamesCfg->getInt("HOUSES","ATREIDES_SWAPCOLOR");
    sHouseInfo[ATREIDES].minimap_color = makeColFromString(gamesCfg->getStr("HOUSES","ATREIDES_MINIMAPCOLOR"));

    // Ordos
    sHouseInfo[ORDOS].swap_color = gamesCfg->getInt("HOUSES","ORDOS_SWAPCOLOR");
    sHouseInfo[ORDOS].minimap_color = makeColFromString(gamesCfg->getStr("HOUSES","ORDOS_MINIMAPCOLOR"));

    // Mercenary
    sHouseInfo[MERCENARY].swap_color = gamesCfg->getInt("HOUSES","MERCENARY_SWAPCOLOR");
    sHouseInfo[MERCENARY].minimap_color = makeColFromString(gamesCfg->getStr("HOUSES","MERCENARY_MINIMAPCOLOR"));

    // Sardaukar
    sHouseInfo[SARDAUKAR].swap_color = gamesCfg->getInt("HOUSES","SARDAUKAR_SWAPCOLOR");
    sHouseInfo[SARDAUKAR].minimap_color = makeColFromString(gamesCfg->getStr("HOUSES","SARDAUKAR_MINIMAPCOLOR"));

    // Fremen
    sHouseInfo[FREMEN].swap_color = gamesCfg->getInt("HOUSES","FREMEN_SWAPCOLOR");
    sHouseInfo[FREMEN].minimap_color = makeColFromString(gamesCfg->getStr("HOUSES","FREMEN_MINIMAPCOLOR"));

    // GREY
    // ???
    sHouseInfo[CORRINO].swap_color = gamesCfg->getInt("HOUSES","CORRINO_SWAPCOLOR");
    sHouseInfo[CORRINO].minimap_color = makeColFromString(gamesCfg->getStr("HOUSES","CORRINO_MINIMAPCOLOR"));
}
