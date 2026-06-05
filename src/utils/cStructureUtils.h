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
