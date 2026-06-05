#include "cStructureFactory.h"

#include "gameobjects/units/cReinforcements.h"
#include "gameobjects/structures/cStructures.h"
#include "context/cInfoContext.h"
#include "context/cGameObjectContext.h"
#include "context/GameContext.hpp"
#include "include/sGameServices.h"
#include "game/cGameInterface.h"
#include "game/cGameSettings.h"
#include "utils/cStructureUtils.h"
#include "gameobjects/map/cMapCamera.h"

#include "cBarracks.h"
#include "cConstYard.h"
#include "cGunTurret.h"
#include "cHeavyFactory.h"
#include "cHighTech.h"
#include "cIx.h"
#include "cLightFactory.h"
#include "cOutPost.h"
#include "cPalace.h"
#include "cRefinery.h"
#include "cRepairFacility.h"
#include "cRocketTurret.h"
#include "cSpiceSilo.h"
#include "cStarPort.h"
#include "cWindTrap.h"
#include "cWor.h"
#include "data/gfxdata.h"
#include "gameobjects/map/cMapEditor.h"
#include "gameobjects/map/MapGeometry.hpp"
#include "gameobjects/players/cPlayer.h"
#include "gameobjects/players/cPlayers.h"
#include "utils/cLog.h"
#include "utils/RNG.hpp"
#include <format>

cStructureFactory::cStructureFactory()
{
}

void cStructureFactory::serviceInit(sGameServices *services)
{
    m_services = services;
    m_objects = services->objects;
    m_info = services->info;
    m_mapCamera = services->mapCamera;
    m_settings = services->settings;
    m_structureUtils = services->structureUtils;
    m_interface = services->ctx->getGameInterface();
}

cStructureFactory::~cStructureFactory()
{
    deleteAllExistingStructures();
}

cAbstractStructure *cStructureFactory::createStructureInstance(int type)
{
    // Depending on type, create proper derived class. The constructor
    // will take care of the rest
    if (type == CONSTYARD)		return new cConstYard;
    if (type == STARPORT)		return new cStarPort;
    if (type == WINDTRAP)		return new cWindTrap;
    if (type == SILO)			return new cSpiceSilo;
    if (type == REFINERY)		return new cRefinery;
    if (type == RADAR)			return new cOutPost;
    if (type == PALACE)			return new cPalace;
    if (type == HIGHTECH)		return new cHighTech;
    if (type == LIGHTFACTORY)      return new cLightFactory;
    if (type == HEAVYFACTORY)      return new cHeavyFactory;
    if (type == TURRET)      return new cGunTurret;
    if (type == RTURRET)      return new cRocketTurret;
    if (type == REPAIR)      return new cRepairFacility;
    if (type == IX)      return new cIx;
    if (type == WOR)      return new cWor;
    if (type == BARRACKS)      return new cBarracks;
    return nullptr;
}

void cStructureFactory::deleteStructureInstance(cAbstractStructure *pStructure)
{
    // delete memory acquired
    m_objects->getStructures()[pStructure->getStructureId()] = nullptr;
    delete pStructure;
}

/**
	Shorter version, creates structure at full health.
**/
// cAbstractStructure *cStructureFactory::createStructure(int iCell, int iStructureType, int iPlayer)
// {
//     return createStructure(iCell, iStructureType, iPlayer, 100);
// }

/**
	Create a structure, place it and return a reference to this created class.

	This method will return nullptr when either an error occurred, or the creation
	of a non-structure type (ie SLAB/WALL) is done.
**/
cAbstractStructure *cStructureFactory::createStructure(int iCell, int iStructureType, int iPlayer, int iPercent)
{
    d2tm_assert(iPlayer >= 0);
    d2tm_assert(iPlayer <= MAX_PLAYERS);

    if (iPercent > 100) iPercent = 100;

    updatePlayerCatalogAndPlaceNonStructureTypeIfApplicable(iCell, iStructureType, iPlayer);
    clearFogForStructureType(iCell, iStructureType, 2, iPlayer);

    // SLAB and WALL is not a real structure. The terrain is manipulated
    // therefore quit here, as we won't place real structure.
    if (iStructureType == SLAB1 || iStructureType == SLAB4 || iStructureType == WALL) {
        return nullptr;
    }

    float fPercent = (float)iPercent/100; // divide by 100 (to make it 0.x)

    s_StructureInfo &structureInfo = m_info->getStructureInfo(iStructureType);
    int hp = structureInfo.hp;
    if (hp < 0) {
        cLogger::getInstance()->log(LOG_INFO, COMP_STRUCTURES, "create structure", "Structure to create has no hp, aborting creation.");
        return nullptr;
    }

    int iNewId = getFreeSlot();

    // fail
    if (iNewId < 0) {
        cLogger::getInstance()->log(LOG_INFO, COMP_STRUCTURES, "create structure", "No free slot available, returning nullptr");
        return nullptr;
    }

    cAbstractStructure *str = createStructureInstance(iStructureType);

    if (str == nullptr) {
        cLogger::getInstance()->log(LOG_INFO, COMP_STRUCTURES, "create structure", "cannot create structure: createStructureInstance returned nullptr");
        return nullptr; // fail
    }

    cPoint absTopLeft = m_objects->getMapGeometry()->getAbsolutePositionFromCell(iCell);
    cPlayer *player = m_objects->getPlayer(iPlayer);

    for (auto flag : structureInfo.flags) {
        cPoint pos = cPoint(
                         absTopLeft.x + flag.relX,
                         absTopLeft.y + flag.relY
                     );

        if (flag.big) {
            str->addFlag(cFlag::createBigFlag(player, pos, m_mapCamera, m_settings));
        }
        else {
            str->addFlag(cFlag::createSmallFlag(player, pos, m_mapCamera, m_settings));
        }
    }

    // calculate actual health
    float fHealth = hp * fPercent;

    cLogger::getInstance()->log(LOG_DEBUG, COMP_STRUCTURES, "Setting win flags", 
            std::format("Structure with id [{}] has [{}] hp , fhealth is [{}]", iStructureType, hp, fHealth));

    int structureSize = structureInfo.bmp_width * structureInfo.bmp_height;

    // assign to array
    str->serviceInit(m_services);
    m_objects->getStructures()[iNewId] = str;

    // Now set it up for location & player
    str->setCell(iCell);
    str->setOwner(iPlayer);
    str->setBuildingFase(1); // prebuild
    str->TIMER_prebuild = std::min(structureSize/16, 250); // prebuild timer. A structure of 64x64 will result in 256, bigger structure has longer timer
    str->TIMER_decay = RNG::rnd(1000) + 100;
    str->fConcrete = (1 - fPercent);
    str->setHitPoints((int)fHealth);
    str->setFrame(RNG::rnd(1)); // random start frame (flag)
    str->setStructureId(iNewId);
    str->setWidth(structureInfo.bmp_width / TILESIZE_WIDTH_PIXELS);
    str->setHeight(structureInfo.bmp_height / TILESIZE_HEIGHT_PIXELS);

    // clear fog around structure
    clearFogForStructureType(iCell, str);

    // additional forces: (UNITS)
    if (iStructureType == REFINERY) {
        REINFORCE(iPlayer, HARVESTER, iCell, -1);
    }

    m_structureUtils->putStructureOnDimension(MAPID_STRUCTURES, str);

    // handle update
    const s_GameEvent event {
        .eventType = eGameEventType::GAME_EVENT_CREATED,
        .data = CommonEvent {
            .entityType = eBuildType::STRUCTURE,
            .entityID = str->getStructureId(),
            .player = str->getPlayer(),
            .entitySpecificType = iStructureType
        }
    };
    m_interface->onNotifyGameEvent(event);

    return str;
}


/**
	If this is a SLAB1, SLAB4, or WALL. Make changes in terrain.
    Also updates player catalog of structure types.
**/
void cStructureFactory::updatePlayerCatalogAndPlaceNonStructureTypeIfApplicable(int iCell, int iStructureType, int iPlayer)
{
    // add this structure to the array of the player (for some score management)
    cPlayer *cPlayer = m_objects->getPlayer(iPlayer);
    cPlayer->increaseStructureAmount(iStructureType);

    auto mapEditor = cMapEditor(m_objects->getMap());

    if (iStructureType == SLAB1) {
        mapEditor.createCell(iCell, TERRAIN_SLAB, 0);
        return; // done
    }

    if (iStructureType == SLAB4) {
        if (m_objects->getMap()->occupiedByUnit(iCell) == false) {
            if (m_objects->getMap()->getCellType(iCell) == TERRAIN_ROCK) {
                mapEditor.createCell(iCell, TERRAIN_SLAB, 0);
            }
        }

        int cellRight = m_objects->getMapGeometry()->getCellRight(iCell);
        if (m_objects->getMap()->occupiedByUnit(cellRight) == false) {
            if (m_objects->getMap()->getCellType(cellRight) == TERRAIN_ROCK) {
                mapEditor.createCell(cellRight, TERRAIN_SLAB, 0);
            }
        }

        int oneRowBelowCell = m_objects->getMapGeometry()->getCellBelow(iCell);
        if (m_objects->getMap()->occupiedByUnit(oneRowBelowCell) == false) {
            if (m_objects->getMap()->getCellType(oneRowBelowCell) == TERRAIN_ROCK) {
                mapEditor.createCell(oneRowBelowCell, TERRAIN_SLAB, 0);
            }
        }

        int rightToRowBelowCell = m_objects->getMapGeometry()->getCellRight(oneRowBelowCell);
        if (m_objects->getMap()->occupiedByUnit(rightToRowBelowCell) == false) {
            if (m_objects->getMap()->getCellType(rightToRowBelowCell) == TERRAIN_ROCK) {
                mapEditor.createCell(rightToRowBelowCell, TERRAIN_SLAB, 0);
            }
        }

        return;
    }

    if (iStructureType == WALL) {
        mapEditor.createCell(iCell, TERRAIN_WALL, 0);
        // change surrounding walls here
        mapEditor.smoothAroundCell(iCell);
        return;
    }
}



/**
	Clear fog around structure, using a cAbstractStructure class.
**/
void cStructureFactory::clearFogForStructureType(int iCell, cAbstractStructure *str)
{
    if (str == nullptr) {
        return;
    }

    clearFogForStructureType(iCell, str->getType(), m_info->getStructureInfo(str->getType()).sight, str->getOwner());
}

/**
	Clear the cells of a structure on location of iCell, the size that is cleared is
	determined from the iStructureType
**/
void cStructureFactory::clearFogForStructureType(int iCell, int iStructureType, int iSight, int iPlayer)
{
    int iWidth = m_info->getStructureInfo(iStructureType).bmp_width / 32;;
    int iHeight = m_info->getStructureInfo(iStructureType).bmp_height / 32;

    int iCellX = m_objects->getMapGeometry()->getCellX(iCell);
    int iCellY = m_objects->getMapGeometry()->getCellY(iCell);
    int iCellXMax = iCellX + iWidth;
    int iCellYMax = iCellY + iHeight;

    for (int x = iCellX; x < iCellXMax; x++) {
        for (int y = iCellY; y < iCellYMax; y++) {
            m_objects->getMap()->clearShroud(m_objects->getMapGeometry()->makeCell(x, y), iSight, iPlayer);
        }
    }
}

/**
	return free slot in array of structures.
**/
int cStructureFactory::getFreeSlot()
{
    for (int i=0; i < MAX_STRUCTURES; i++) {
        if (m_objects->getStructures()[i] == nullptr) {
            return i;
        }
    }

    return -1; // NONE
}

/**
<p>
 This function will return how many (if any) slabs are found for the surface of a structure type. This function
 does *not* check if a structure can be placed at this location.
 </p>
<p>
	Ie, given a constyard of 2x2 (4 cells), this can return 0 (no slabs) or 4 (fully slabbed) or in between.
</p>
 <p>
 <b>Returns:</b><br>
 Any amount of cells that are "slabbed".
 </p>
**/
int cStructureFactory::getSlabStatus(int iCell, int iStructureType)
{
    if (!m_objects->getMapGeometry()->isValidCell(iCell)) return 0;

    // checks if this structure can be placed on this cell
    int w = m_info->getStructureInfo(iStructureType).bmp_width / TILESIZE_WIDTH_PIXELS;
    int h = m_info->getStructureInfo(iStructureType).bmp_height / TILESIZE_HEIGHT_PIXELS;

    int slabs = 0;

    int x = m_objects->getMapGeometry()->getCellX(iCell);
    int y = m_objects->getMapGeometry()->getCellY(iCell);

    for (int cx = 0; cx < w; cx++) {
        for (int cy = 0; cy < h; cy++) {
            int cll = m_objects->getMapGeometry()->getCellWithMapBorders(cx + x, cy + y);

            if (cll < 0) {
                continue;
            }

            // If the 'terrain' type is 'slab' increase value of found slabs.
            if (m_objects->getMap()->getCellType(cll) == TERRAIN_SLAB) {
                slabs++;
            }
        }
    }

    return slabs; // ranges from 0 to <max slabs possible of building (ie height * width in cells)
}

void cStructureFactory::createSlabForStructureType(int iCell, int iStructureType)
{
    // DUPLICATED BY slabStructure?
    d2tm_assert(iCell > -1);

    int height = m_info->getStructureInfo(iStructureType).bmp_height / 32;
    int width = m_info->getStructureInfo(iStructureType).bmp_width / 32;

    int cellX = m_objects->getMapGeometry()->getCellX(iCell);
    int cellY = m_objects->getMapGeometry()->getCellY(iCell);

    int endCellX = cellX + width;
    int endCellY = cellY + height;
    auto mapEditor = cMapEditor(m_objects->getMap());
    for (int y = cellY; y < endCellY; y++) {
        for (int x = cellX; x < endCellX; x++) {
            int cell = m_objects->getMapGeometry()->getCellWithMapDimensions(x, y);
            mapEditor.createCell(cell, TERRAIN_SLAB, 0);
        }
    }
}

void cStructureFactory::deleteAllExistingStructures()
{
    if (m_objects == nullptr)
        return;
    for (int i=0; i < MAX_STRUCTURES; i++) {
        // clear out all structures
        cAbstractStructure *pStructure = m_objects->getStructures()[i];
        if (pStructure) {
            delete pStructure;
        }
        // clear pointer
        m_objects->getStructures()[i] = nullptr;
    }
}

void cStructureFactory::slabStructure(int iCll, int iStructureType, int iPlayer)
{
    const s_StructureInfo &sStructures = m_info->getStructureInfo(iStructureType);

    int width = sStructures.bmp_width / TILESIZE_WIDTH_PIXELS;
    int height = sStructures.bmp_height / TILESIZE_HEIGHT_PIXELS;

    int x = m_objects->getMapGeometry()->getCellX(iCll);
    int y = m_objects->getMapGeometry()->getCellY(iCll);

    int endX = x + width;
    int endY = y + height;

    for (int sx = x; sx < endX; sx++) {
        for (int sy = y; sy < endY; sy++) {
            createStructure(m_objects->getMapGeometry()->getCellWithMapBorders(sx, sy), SLAB1, iPlayer);
        }
    }
}