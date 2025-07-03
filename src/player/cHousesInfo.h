#pragma once

#include <memory>
#include "utils/cIniFile.h"
#include <SDL2/SDL.h>
#include "utils/Color.hpp"

#define MAX_HOUSES        12      // 12 different type of houses

// House properties
struct s_HouseInfo {
    int swap_color;           // color index to start swapping with.
    Color minimap_color;        // rgb value on minimap
};

class cHousesInfo {
public:
    cHousesInfo() = default;

    ~cHousesInfo() = default;

    int getSwapColor(int house) {
        return m_houseInfo[house].swap_color;
    }

    Color getMinimapColor(int house) {
        return m_houseInfo[house].minimap_color;
    }

    void INSTALL_HOUSES(std::shared_ptr<cIniFile> gameCfg);

private:
    s_HouseInfo m_houseInfo[MAX_HOUSES];
    Color makeColFromString(std::string colorStr);
};

