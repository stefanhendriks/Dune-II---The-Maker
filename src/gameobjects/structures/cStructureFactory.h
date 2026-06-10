/*

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefan@fundynamic.com
  Website: http://dune2themaker.fundynamic.com

  2001 - 2022 (c) code by Stefan Hendriks

  */

#pragma once

#include "cAbstractStructure.h"

class cGameObjectContext;
class cInfoContext;
class cMapCamera;
class cGameInterface;
class cGameSettings;
class cStructureUtils;
class SDLDrawer;
struct sGameServices;

class cStructureFactory {
public:
    cStructureFactory();

    ~cStructureFactory();

    void serviceInit(sGameServices *services);

    void deleteStructureInstance(cAbstractStructure *pStructure);

    cAbstractStructure *createStructure(int iCell, int iStructureType, int iPlayer, int iPercent = 100);

    // cAbstractStructure *createStructure(int iCell, int iStructureType, int iPlayer);

    int getFreeSlot();

    void deleteAllExistingStructures();

    void clearFogForStructureType(int iCell, cAbstractStructure *str);

    void clearFogForStructureType(int iCell, int iStructureType, int iSight, int iPlayer);

    void createSlabForStructureType(int iCell, int iStructureType);

    void updatePlayerCatalogAndPlaceNonStructureTypeIfApplicable(int iCell, int iStructureType, int iPlayer);

    int getSlabStatus(int iCell, int iStructureType);

    void slabStructure(int iCll, int iStructureType, int iPlayer);

private:
    cGameObjectContext *m_objects = nullptr;
    cInfoContext *m_info = nullptr;
    cMapCamera *m_mapCamera = nullptr;
    cGameInterface *m_interface = nullptr;
    cGameSettings *m_settings = nullptr;
    cStructureUtils *m_structureUtils = nullptr;
    sGameServices *m_services = nullptr;
    SDLDrawer *m_renderer = nullptr;

    cAbstractStructure *createStructureInstance(int type);
};
