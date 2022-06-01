#pragma once

#include <memory>
#include "utils/cIniFile.h"

#define MAX_HOUSES        12      // 12 different type of houses

// House properties
struct s_HouseInfo {
    int swap_color;           // color index to start swapping with.
    int minimap_color;        // rgb value on minimap
};

class cHousesInfo {
public:
    cHousesInfo() = default;

    ~cHousesInfo() = default;

    int getSwapColor(int house) { return m_houseInfo[house].swap_color; }

    int getMinimapColor(int house) { return m_houseInfo[house].minimap_color; }

    void INSTALL_HOUSES(std::shared_ptr<cIniFile> gameCfg);

private:
    s_HouseInfo m_houseInfo[MAX_HOUSES];
};

