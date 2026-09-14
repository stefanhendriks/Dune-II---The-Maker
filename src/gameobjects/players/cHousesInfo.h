/**
 * @file cHousesInfo.h
 *
 * Dune 2 - The Maker
 *
 * @author Stefan Hendriks & the D2TM Team
 * @www http://www.dune2themaker.com
 * @copyright Copyright (c) 2001 - 2026 D2TM Team
 * @license This software is released under the MIT License.
 *
 * Note: Dune 2 is a trademark of Westwood Studios/Electronic Arts.
 *
 * This is a non-commercial educational project.
 */

#pragma once

#include <memory>
#include "utils/cIniFile.h"
#include <SDL3/SDL.h>
#include "utils/Color.hpp"
#include "utils/HouseColors.h"

#define MAX_HOUSES        12      // 12 different type of houses

// House properties
struct s_HouseInfo {
    Color minimap_color;      // rgb value on minimap
    HouseColors colors;         // 7 RGB entries used to recolor sprites
};

class cHousesInfo {
public:
    cHousesInfo();

    ~cHousesInfo() = default;

    const HouseColors& getColors(int house) const {
        return m_houseInfo[house].colors;
    }

    Color getMinimapColor(int house) const {
        return m_houseInfo[house].minimap_color;
    }

    void installHouses(std::shared_ptr<cIniFile> gameCfg);

private:
    s_HouseInfo m_houseInfo[MAX_HOUSES];
};

