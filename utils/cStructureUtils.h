/*
 * cStructureUtils.h
 *
 *  Created on: 2-aug-2010
 *      Author: Stefan
 */

#ifndef CSTRUCTUREUTILS_H_
#define CSTRUCTUREUTILS_H_

class cStructureUtils {
public:
    cStructureUtils();

    ~cStructureUtils();

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
};

#endif /* CSTRUCTUREUTILS_H_ */
