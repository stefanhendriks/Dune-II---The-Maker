/**
 * @file cStructureUtils.h
 *
 * Dune 2 - The Maker
 *
 * @author Stefan Hendriks & the D2TM Team
 * @www http://www.dune2themaker.com
 * @copyright Copyright (c) 2002 - 2026 Stefan Hendriks and contributors
 * @license This software is released under the MIT License. See LICENSE.md.
 *
 * Note: Dune 2 is a trademark of Westwood Studios/Electronic Arts.
 *
 * This is a non-commercial educational project.
 */

#pragma once

#include "gameobjects/structures/cAbstractStructure.h"
#include "sidebar/cBuildingListItem.h"

class cPlayer;
struct sGameServices;
class cGameObjectContext;
class cInfoContext;
class cMapCamera;
class cGameSettings;

class cStructureUtils {
public:
    cStructureUtils();

    ~cStructureUtils();

    void serviceInit(sGameServices* services);

    int findStructureToDeployUnit(cPlayer *pPlayer, int structureType);

    int findStarportToDeployUnit(cPlayer *pPlayer);

    int findHiTechToDeployAirUnit(cPlayer *pPlayer);

    int findStructureBy(int iPlayer, int iType, bool bFreeAround);

    int findStructureTypeByTypeOfList(cBuildingListItem *item);

    int getHeightOfStructureTypeInCells(int structureType);

    int getWidthOfStructureTypeInCells(int structureType);

    void putStructureOnDimension(int dimensionId, cAbstractStructure *theStructure);

    bool isStructureVisibleOnScreen(cAbstractStructure *structure);

    bool isMouseOverStructure(cAbstractStructure *structure, int screenX, int screenY);

    int getTotalPowerOutForPlayer(cPlayer *pPlayer);

    int getTotalPowerUsageForPlayer(cPlayer *pPlayer);

    int getTotalSpiceCapacityForPlayer(cPlayer *pPlayer);

    int getStructureTypeByUnitBuildId(int unitBuildId) const;

private:
    cGameObjectContext* m_objects = nullptr;
    cInfoContext* m_infos = nullptr;
    cMapCamera* m_mapCamera = nullptr;
    cGameSettings* m_settings = nullptr;
};
