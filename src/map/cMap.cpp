/*

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefan@fundynamic.com
  Website: http://dune2themaker.fundynamic.com

  2001 - 2022 (c) code by Stefan Hendriks

  */

#include "cMap.h"
#include "gameobjects/structures/cAbstractStructure.h"
#include "cMapCamera.h"
#include "cMapEditor.h"
#include "game/cGameInterface.h"
#include "utils/cLog.h"
#include "data/gfxdata.h"
#include "gameobjects/particles/cParticle.h"
#include "gameobjects/projectiles/bullet.h"
#include "gameobjects/structures/cStructureFactory.h"
#include "gameobjects/particles/cParticles.h"
#include "gameobjects/structures/cStructures.h"
#include "gameobjects/projectiles/cBullets.h"
#include "player/cPlayer.h"
#include "player/cPlayers.h"
#include "gameobjects/units/cReinforcements.h"
#include "utils/RNG.hpp"
#include "map/MapGeometry.hpp"
#include "context/GameContext.hpp"
#include "gameobjects/sTerrainInfo.h"
#include "utils/d2tm_math.h"
#include "controls/cGameControlsContext.h"
#include "context/cInfoContext.h"
#include "context/cGameObjectContext.h"
#include "gameobjects/units/cUnit.h"
// #include "gameobjects/units/cUnits.h"
#include "drawers/cTextDrawer.h"
#include "include/sGameServices.h"

#include <format>
#include <algorithm>
#include <cmath>

cMap::cMap()
{
    m_TIMER_scroll = 0;
    m_iScrollSpeed = 1;
    m_maxCells = 0;
    m_reinforcements = nullptr;
    m_iDesiredAmountOfWorms = 0;
    m_iTIMER_respawnSandworms = -1;
    m_mapGeometry = std::make_unique<MapGeometry>(64,64);
    init(64, 64);
    m_terrainInfo = nullptr;

    m_settings = nullptr;
    m_infos = nullptr;
    m_objects = nullptr;
    m_interface = nullptr;
    m_log = nullptr;
}

cMap::~cMap() = default;

MapGeometry &cMap::getGeometry() const
{
    return *m_mapGeometry;
}

// void cMap::setGameContext(GameContext* ctx)
// {
//     m_ctx = ctx;
//     m_textDrawer = ctx->getTextContext()->getBeneTextDrawer();
// }

void cMap::serviceInit(sGameServices* services)
{
    assert(services != nullptr);
    m_ctx = services->ctx;
    assert(m_ctx != nullptr);
    m_log = m_ctx->getLog();
    assert(m_log != nullptr);

    m_textDrawer = m_ctx->getTextContext()->getBeneTextDrawer();
    assert(m_textDrawer != nullptr);

    m_settings = services->settings;
    assert(m_settings != nullptr);
    m_infos = services->info;
    assert(m_infos != nullptr);
    m_objects = services->objects;
    assert(m_objects != nullptr);
    m_interface = m_ctx->getGameInterface();
    assert(m_interface != nullptr);
}

void cMap::setReinforcements(std::shared_ptr<cReinforcements> reinforcements)
{
    m_reinforcements = reinforcements;
}

void cMap::setTerrainInfo(s_TerrainInfo* terrainInfo)
{
    m_terrainInfo = terrainInfo;
}

void cMap::init(int width, int height)
{
    if (m_reinforcements) {
        m_reinforcements->init();
    }

    m_bAutoDetonateSpiceBlooms = false;
    m_bAutoSpawnSpiceBlooms = true;

    m_iTIMER_blooms = 200;
    m_mBloomTimers = std::map<int, int>();
    m_mBloomTimers.clear();

    m_iDesiredAmountOfWorms = 0;
    m_iTIMER_respawnSandworms = -1;

    m_maxCells = width * height;
    m_cell = std::vector<tCell>(m_maxCells, tCell());

    // clear out all cells
    clearAllCells();

    if (m_objects) {
        m_objects->getStructureFactory()->deleteAllExistingStructures();
    }

    m_TIMER_scroll = 0;
    m_iScrollSpeed = 1;

    m_width = width;
    m_height = height;
    m_mapGeometry->resize(m_width, m_height);
}

void cMap::smudge_increase(SmudgeType iType, int iCell)
{
    if (!isValidCell(iCell)) return;
    tCell *pCell = getCell(iCell);

    if ( !pCell->smudgetype.has_value() )
        pCell->smudgetype = iType;

    if (pCell->smudgetype == SmudgeType::S_WALL)
        pCell->smudgetile = 0;

    if (pCell->smudgetype == SmudgeType::S_ROCK) {
        if (pCell->smudgetile < 0)
            pCell->smudgetile = RNG::rnd(2);
        else if (pCell->smudgetile + 2 < 6)
            pCell->smudgetile += 2;
    }

    if (pCell->smudgetype == SmudgeType::S_SAND) {
        if (pCell->smudgetile < 0)
            pCell->smudgetile = RNG::rnd(2);
        else if (pCell->smudgetile + 2 < 6)
            pCell->smudgetile += 2;
    }
}

/**
 * Is this cell occupied? This returns true when type of terrain is WALL or MOUNTAIN.
 *
 * @param iCell
 * @return
 */
bool cMap::occupiedByWallOrMountain(int iCell)
{
    if (iCell < 0 || iCell >= m_maxCells) return false;

    if (getCellType(iCell) == TERRAIN_WALL) return true;
    if (getCellType(iCell) == TERRAIN_MOUNTAIN) return true;

    return false;
}

bool cMap::occupiedInDimension(int iCell, int dimension)
{
    if (!isValidCell(iCell)) return false;
    if (dimension < 0 || dimension >= MAPID_MAX) return false;

    return m_cell[iCell].id[dimension] > -1;
}

bool cMap::occupiedByUnit(int iCell)
{
    return occupiedInDimension(iCell, MAPID_UNITS);
}

/**
 * Is the specific cell occupied by any dimension?
 *
 * @param iCll
 * @return
 */
bool cMap::occupied(int iCell)
{
    if (iCell < 0 || iCell >= m_maxCells) return false;

    if (occupiedInDimension(iCell, MAPID_UNITS)) return true;
    if (occupiedInDimension(iCell, MAPID_AIR)) return true;
    if (occupiedInDimension(iCell, MAPID_STRUCTURES)) return true;
    if (occupiedByWallOrMountain(iCell)) return true;

    return false;
}

bool cMap::canDeployUnitTypeAtCell(int iCell, int iUnitType)
{
    if (iCell < 0 || iUnitType < 0)
        return false;

    s_UnitInfo &unitToDeploy = m_infos->getUnitInfo(iUnitType);

    bool isAirbornUnit = unitToDeploy.airborn;
    bool isInfantryUnit = unitToDeploy.infantry;
    bool isWorm = (iUnitType == SANDWORM);

    if (isAirbornUnit) return true;

    if (isWorm) {
        int wormId = getCellIdWormsLayer(iCell);
        return isCellPassableForWorm(iCell) && wormId < 0;
    }

    if (isInfantryUnit && isCellPassableForFootUnits(iCell)) return true;

    if (!isCellPassable(iCell)) return false;

    int strucId = getCellIdStructuresLayer(iCell);
    int unitId = getCellIdUnitLayer(iCell);

    return unitId < 0 && strucId < 0;
}

/**
 * Checks if a cary-all (passed with iUnitIDWithinStructure) can deploy a unit at iCll.
 *
 * @param iCll (cell to deploy)
 * @param iUnitID (the carry-all)
 * @return
 */
bool cMap::canDeployUnitAtCell(int iCell, int iUnitID)
{
    if (iCell < 0 || iUnitID < 0)
        return false;

    cUnit *pUnit = m_objects->getUnit(iUnitID);
    if (!pUnit) return false;
    if (!pUnit->isAirbornUnit()) return false; // weird unit passed in
    if (pUnit->iNewUnitType < 0) return false; // safe-guard when this unit has no new unit to spawn

    int structureIdOnMap = getCellIdStructuresLayer(iCell);
    if (structureIdOnMap > -1) {
        // the cell contains a structure that the unit wants to enter (for repairment?)
        if (pUnit->iStructureID > -1) {
            if (structureIdOnMap == pUnit->iStructureID) {
                return true;
            }
        }

        // all other cases are occupied / blocked
        return false;
    }

    s_UnitInfo &unitToDeploy = m_infos->getUnitInfo(pUnit->iNewUnitType);

    bool isAirbornUnit = unitToDeploy.airborn;
    bool isInfantryUnit = unitToDeploy.infantry;

    if (!isAirbornUnit) {
        int cellIdOnMap = getCellIdUnitLayer(iCell);
        if (cellIdOnMap > -1 && cellIdOnMap != iUnitID) {
            return false; // other unit at cell
        }
    }

    // walls block as do mountains
    if (getCellType(iCell) == TERRAIN_WALL) {
        return false;
    }

    // mountains only block infantry
    if (getCellType(iCell) == TERRAIN_MOUNTAIN) {
        // we can deploy infantry types on mountains, airborn units can fly over
        if (!isInfantryUnit && !isAirbornUnit) {
            return false;
        }
    }

    return true;
}

/**
 * Returns true if cell is occupied, for a given unit. The unitId is given so it can ignore its own ID if that
 * is recorded on map data. (ie when moving, a unit can occupy 2 cells at once).
 * @param iCll
 * @param iUnitID
 * @return
 */
bool cMap::occupied(int iCll, int iUnitID)
{
    if (iCll < 0 || iUnitID < 0)
        return true;

    cUnit *pUnit = m_objects->getUnit(iUnitID);
    if (!pUnit) return true;

    int structureIdOnMap = getCellIdStructuresLayer(iCll);
    if (structureIdOnMap > -1) {
        // the cell contains a structure that the unit wants to enter
        if (pUnit->iStructureID > -1) {
            if (structureIdOnMap == pUnit->iStructureID) {
                // the unit wants to enter this structure, so it does not block the unit
                return false;
            }
        }

        // all other cases are occupied / blocked
        return true;
    }

    // non airborn units can block each other
    if (!pUnit->isAirbornUnit() && !pUnit->isSandworm()) {
        int cellIdOnMap = getCellIdUnitLayer(iCll);
        if (cellIdOnMap > -1 && cellIdOnMap != iUnitID) {
            return true; // other unit at cell
        }
    }

    // walls block as do mountains
    if (getCellType(iCll) == TERRAIN_WALL) {
        return true;
    }

    // mountains only block infantry
    if (getCellType(iCll) == TERRAIN_MOUNTAIN) {
        if (!pUnit->isInfantryUnit() && !pUnit->isAirbornUnit()) {
            return true;
        }
    }

    return false;
}

/**
 * Think function, called every 5 ms
 */
void cMap::thinkFast()
{
    thinkAutoDetonateSpiceBlooms();
    thinkAboutSpawningNewSpiceBlooms();
    thinkAboutRespawningWorms();
}

void cMap::thinkAboutRespawningWorms()
{
    if (m_iTIMER_respawnSandworms < 0) return;

    if (m_iTIMER_respawnSandworms > 1) {
        m_iTIMER_respawnSandworms--;
        return;
    }

    // timer hit exactly '1'
    m_iTIMER_respawnSandworms--;

    int currentAmountOfWorms = m_objects->getPlayer(AI_WORM)->getAmountOfUnitsForType(SANDWORM);
    if (currentAmountOfWorms < m_iDesiredAmountOfWorms) {
        // spawn one worm, set timer again
        int failures = 0;
        while (failures < 10) {
            int cell = m_mapGeometry->getRandomCellWithinMapWithSafeDistanceFromBorder(2);
            if (!isCellPassableForWorm(cell)) {
                failures++;
                continue;
            }
            logbook(std::format("cMap::thinkAboutRespawningWorms : Spawning sandworm at {}", cell));
            //cUnits::unitCreate(cell, SANDWORM, AI_WORM, true);
             const s_GameEvent event {
                .eventType = eGameEventType::GAME_EVENT_CREATE_UNIT,
                .data = DeployUnitEvent {
                    .iCell = cell,
                    .unitType = SANDWORM,
                    .iPlayer = AI_WORM,
                    .bOnStart = true
                }
            };
            m_interface->onNotifyGameEvent(event);
            break;
        }
    }

    // we have the desired amount of worms already so don't do anything
}

void cMap::thinkAboutSpawningNewSpiceBlooms()
{
    if (!m_bAutoSpawnSpiceBlooms) {
        return;
    }

    if (m_iTIMER_blooms > 0) {
        m_iTIMER_blooms--;
        return;
    }

    // Evaluate every 30 seconds orso
    m_iTIMER_blooms = m_terrainInfo->bloomTimerDuration * 30;

    const std::vector<int> &blooms = getAllCellsOfType(TERRAIN_BLOOM);
    int totalSpiceBloomsCount = blooms.size();

    // When no blooms are detected, we must 'spawn' one
    // @mira: do better than m_width*m_height>64*64
    int desiredAmountOfSpiceBloomsInMap = (m_width*m_height>64*64) ? 6 : 3;

    if (totalSpiceBloomsCount < desiredAmountOfSpiceBloomsInMap) {
        // randomly create a new spice bloom somewhere on the map
        int iCll = -1;
        for (int i = 0; i < 10; i++) {
            int cell = m_mapGeometry->getRandomCell();
            // find place to spawn bloom
            if (getCellType(cell) == TERRAIN_SAND) {
                iCll = cell;
                break;
            }
        }

        if (iCll > -1) {
            // create bloom (can deal with < -1 cell)
            cMapEditor(*this).createCell(iCll, TERRAIN_BLOOM, 0);
        }
        else {
            m_iTIMER_blooms = (1000 / 5) * 5; // try again sooner than 30 secs
        }
    }
}

void cMap::thinkAutoDetonateSpiceBlooms()  // let spice bloom detonate after X amount of time
{
    if (!m_bAutoDetonateSpiceBlooms) {
        return;
    }

    std::vector<int> keys;
    for (auto const &pair: m_mBloomTimers) {
        keys.push_back(pair.first);
    }

    // key == cell
    for (auto const &key: keys) {
        m_mBloomTimers[key] -= 1; // decrease timer
        if (m_mBloomTimers[key] < 1) {
            // detonate spice bloom
            detonateSpiceBloom(key);
        }
    }
}

void cMap::draw_bullets()
{
    // Loop through all units, check if they should be drawn, and if so, draw them
    for (auto& bullet : m_objects->getBullets()) {
        if (bullet.bAlive) {
            bullet.draw();
        }
    }
}

void cMap::clear_all(int playerId)
{
    for (int c = 0; c < m_maxCells; c++) {
        m_cell[c].iVisible[playerId] = true;
    }
}

void cMap::clearShroudForAllPlayers(int c, int size)
{
    for (int p = 0; p < MAX_PLAYERS; p++) {
        clearShroud(c, size, p);
    }
}

void cMap::clearShroud(int c, int size, int playerId)
{
    if (!m_mapGeometry->isWithinBoundaries(c)) return;

    auto *mapCamera = m_interface->getMapCamera();

    setVisibleFor(c, playerId);

    // go around 360 fDegrees and calculate new stuff.
    for (float dr = 1; dr < size; dr++) {
        for (float d = 0; d < 360; d++) { // if we reduce the amount of degrees, we don't get full coverage.
            // need a smarter way to do this (less CPU intensive).

            int x = m_mapGeometry->getAbsoluteXPositionFromCellCentered(c);
            int y = m_mapGeometry->getAbsoluteYPositionFromCellCentered(c);

            float dr1 = cos(d) * (dr * TILESIZE_WIDTH_PIXELS);
            float dr2 = sin(d) * (dr * TILESIZE_HEIGHT_PIXELS);

            x = (x + dr1);
            y = (y + dr2);

            // convert back
            int cl = mapCamera->getCellFromAbsolutePosition(x, y);

            if (cl < 0) continue;

            if (!isVisible(cl, playerId)) {
                setVisibleFor(cl, playerId);

                int structureId = getCellIdStructuresLayer(cl);
                if (structureId > -1) {
                    cAbstractStructure *pStructure = m_objects->getStructure(structureId);
                    if (!pStructure) continue;
                    const s_GameEvent event {
                        .eventType = eGameEventType::GAME_EVENT_DISCOVERED,
                        .data = CommonEvent {
                            .entityType = eBuildType::STRUCTURE,
                            .entityID = structureId,
                            .player = m_objects->getPlayer(playerId),
                            .entitySpecificType = pStructure->getType(),
                            .atCell = cl
                        }
                    };
                    m_interface->onNotifyGameEvent(event);
                }

                int unitId = getCellIdUnitLayer(cl);
                if (unitId > -1) {
                    cUnit *cUnit = m_objects->getUnit(unitId);
                    if (cUnit && cUnit->isValid()) {
                        s_GameEvent event {
                            .eventType = eGameEventType::GAME_EVENT_DISCOVERED,
                            .data = CommonEvent {
                                .entityType = eBuildType::UNIT,
                                .entityID = unitId,
                                .player = m_objects->getPlayer(playerId),
                                .entitySpecificType = cUnit->getType(),
                                .atCell = cl
                            }
                        };

                        m_interface->onNotifyGameEvent(event);
                    }
                }
            } // make visible
        }
    }
}

// Each index is a map data field holding indexes of the map layout
//
void cMap::remove_id(int iIndex, int iIDType)
{
    // Search through the entire map and remove the id
    for (int iCell = 0; iCell < m_maxCells; iCell++) {
        tCell &tCell = m_cell[iCell];
        if (tCell.id[iIDType] == iIndex) {
            tCell.id[iIDType] = -1;
        }
    }
}

void cMap::draw_units()
{
    auto *mapViewport = m_interface->getMapViewport();

    //@Mira SDL2 blender
    //// @Mira fix trasnparency set_trans_blender(0, 0, 0, 160);

    // draw all worms first
    for (int i = 0; i < m_objects->getUnitsSize(); i++) {
        cUnit *pUnit = m_objects->getUnit(i);
        if (!pUnit || !pUnit->isValid()) continue;

        // DEBUG MODE: DRAW PATHS
        if (m_settings->isDrawUnitDebug()) {
            pUnit->draw_path();
        }

        if (pUnit->iType != SANDWORM) continue;

        if (pUnit->isWithinViewport(mapViewport)) {
            pUnit->draw();
        }

        drawUnitDebug(pUnit);

    }

    // then: draw infantry units
    for (int i = 0; i < m_objects->getUnitsSize(); i++) {
        cUnit *pUnit = m_objects->getUnit(i);
        if (!pUnit || !pUnit->isValid()) continue;

        if (!pUnit->isInfantryUnit())
            continue; // skip non-infantry units

        if (pUnit->isWithinViewport(mapViewport)) {
            // draw
            pUnit->draw();
        }

        drawUnitDebug(pUnit);
    }

    // then: draw ground units
    for (int i = 0; i < m_objects->getUnitsSize(); i++) {
        cUnit *pUnit = m_objects->getUnit(i);
        if (!pUnit || !pUnit->isValid()) continue;

        if (pUnit->isAirbornUnit() ||
                pUnit->isSandworm() ||
                pUnit->isInfantryUnit())
            continue; // skip airborn, infantry and sandworm

        if (pUnit->isWithinViewport(mapViewport)) {
            // draw
            pUnit->draw();
        }

        drawUnitDebug(pUnit);
    }
}

void cMap::drawUnitDebug(cUnit *pUnit) const
{
    if (!pUnit) return;
    if (!m_settings->isDrawUnitDebug()) return;

    pUnit->draw_debug(m_textDrawer);
}

// draw 2nd layer for units, this is health/spice bars and eventually airborn units (last)
void cMap::draw_units_2nd()
{
    auto *mapViewport = m_interface->getMapViewport();

    // draw health of units
    for (int i = 0; i < m_objects->getUnitsSize(); i++) {
        cUnit *pUnit = m_objects->getUnit(i);
        if (!pUnit || !pUnit->isValid()) continue;
        if (!pUnit->rendering.bHovered && !pUnit->isSelected()) continue;
        if (!pUnit->isWithinViewport(mapViewport)) continue;
        if (pUnit->isHidden()) continue;

        pUnit->draw_health();
        pUnit->draw_group(m_textDrawer);
        pUnit->draw_experience();
        if (pUnit->iType == HARVESTER) {
            pUnit->draw_spice();
        }
    }

    // draw airborn units
    for (int i = 0; i < m_objects->getUnitsSize(); i++) {
        cUnit *pUnit = m_objects->getUnit(i);
        if (!pUnit || !pUnit->isValid()) continue;
        if (!pUnit->isAirbornUnit()) continue;

        if (pUnit->isWithinViewport(mapViewport)) {
            pUnit->draw();
            // TODO: Only human players?
            pUnit->draw_health();
            if (m_settings->isDebugMode()) {
                drawUnitDebug(pUnit);
            }
        }

    }
}

int cMap::mouse_draw_x()
{
    cPlayer *humanPlayer = m_objects->getPlayer(HUMAN);
    if (humanPlayer->getGameControlsContext()->getMouseCell() > -1) {
        int mouseCell = humanPlayer->getGameControlsContext()->getMouseCell();
        int absX = m_mapGeometry->getAbsoluteXPositionFromCell(mouseCell);
        return m_interface->getMapCamera()->getWindowXPosition(absX);
    }
    return -1;
}

int cMap::mouse_draw_y()
{
    //@mira rewrite
    cPlayer *humanPlayer = m_objects->getPlayer(HUMAN);
    if (humanPlayer->getGameControlsContext()->getMouseCell() > -1) {
        int mouseCell = humanPlayer->getGameControlsContext()->getMouseCell();
        int absY = m_mapGeometry->getAbsoluteYPositionFromCell(mouseCell);
        return m_interface->getMapCamera()->getWindowYPosition(absY);
    }
    return -1;
}

/**
 * Return amount of cells of a specific type. (SLOW!)
 *
 * TODO: This can be sped up quite a lot, by letting the mapEditor remember what it has placed (keep counter, or list, etc)
 *
 * @param cellType
 * @return
 */
int cMap::getTotalCountCellType(int cellType)
{
    return getAllCellsOfType(cellType).size();
}

/**
 * Returns cell nr's which are of a specific cellType
 *
 * NOTE: This is a slow method, as it iterates though all possible cells
 * @return
 */
std::vector<int> cMap::getAllCellsOfType(int cellType)
{
    std::vector<int> cellsOfType = std::vector<int>();
    for (int c = 0; c < m_maxCells; c++) {
        if (getCellType(c) == cellType) {
            cellsOfType.push_back(c);
        }
    }
    return cellsOfType;
}

int cMap::getCellSlowDown(int iCell)
{
    int cellType = getCellType(iCell);

    if (cellType == TERRAIN_SAND) return 2;
    if (cellType == TERRAIN_MOUNTAIN) return 5;
    if (cellType == TERRAIN_HILL) return 3;
    if (cellType == TERRAIN_SPICEHILL) return 3;
    if (cellType == TERRAIN_ROCK) return 1;
    if (cellType == TERRAIN_SLAB) return 0;

    return 1;
}

int cMap::findCloseMapBorderCellRelativelyToDestinationCel(int destinationCell)
{
    assert(destinationCell > -1);
    // Cell x and y coordinates
    int iCllX = m_mapGeometry->getCellX(destinationCell);
    int iCllY = m_mapGeometry->getCellY(destinationCell);

    // STEP 1: determine starting
    int iStartCell = -1;
    int lDistance = 9999;

    int tDistance = 9999;
    int cll = -1;

    // HORIZONTAL cells
    for (int iX = 0; iX < m_width; iX++) {
        // check when Y = 0 (top)
        tDistance = m_mapGeometry->distance(iX, 0, iCllX, iCllY);

        if (tDistance < lDistance) {
            lDistance = tDistance;

            cll = m_mapGeometry->makeCell(iX, 0);

            if (occupied(cll) == false) {
                iStartCell = cll;
            }
        }

        // check when Y = map_height (bottom)
        tDistance = m_mapGeometry->distance(iX, m_height - 1, iCllX, iCllY);

        if (tDistance < lDistance) {
            lDistance = tDistance;

            cll = m_mapGeometry->makeCell(iX, m_height - 1);

            if (occupied(cll) == false) {
                iStartCell = cll;
            }
        }
    }

    // VERTICAL cells
    for (int iY = 0; iY < m_height; iY++) {
        // check when X = 0 (left)
        tDistance = m_mapGeometry->distance(0, iY, iCllX, iCllY);

        if (tDistance < lDistance) {
            lDistance = tDistance;

            cll = m_mapGeometry->makeCell(0, iY);

            if (occupied(cll) == false) {
                iStartCell = cll;
            }
        }

        // check when XY = map_m_width (bottom)
        tDistance = m_mapGeometry->distance(m_width - 1, iY, iCllX, iCllY);

        if (tDistance < lDistance) {
            lDistance = tDistance;
            cll = m_mapGeometry->makeCell(m_width - 1, iY);

            if (occupied(cll) == false) {
                iStartCell = cll;
            }
        }
    }

    return iStartCell;
}

bool cMap::isValidCell(int c) const //rip
{
    return m_mapGeometry->isValidCell(c);
}

void cMap::createCell(int cell, int terrainType, int tile)
{
    if (!isValidCell(cell)) return;
    if (terrainType > TERRAIN_WALL) return;
    if (terrainType < TERRAIN_BLOOM) return;
    if (tile < 0) return;
    if (tile > 16) return;

    assert(terrainType >= TERRAIN_BLOOM);
    assert(terrainType <= TERRAIN_WALL);
    assert(tile > -1);
    assert(tile < 17);

    // Set
    cellChangeType(cell, terrainType);
    cellChangeTile(cell, tile);
    cellChangeCredits(cell, 0);
    cellChangeHealth(cell, 0);

    cellChangePassable(cell, true);
    cellChangePassableFoot(cell, true);

    cellChangeSmudgeTile(cell, -1);
    cellChangeSmudgeType(cell, std::nullopt);

    if (terrainType == TERRAIN_SPICE) {
        cellChangeCredits(cell, RNG::genInt(m_terrainInfo->terrainSpiceMinSpice,m_terrainInfo->terrainSpiceMaxSpice));
    }
    else if (terrainType == TERRAIN_SPICEHILL) {
        cellChangeCredits(cell, RNG::genInt(m_terrainInfo->terrainSpiceHillMinSpice,m_terrainInfo->terrainSpiceHillMaxSpice));
    }
    else if (terrainType == TERRAIN_MOUNTAIN) {
        cellChangePassable(cell, false);
        cellChangePassableFoot(cell, true);
    }
    else if (terrainType == TERRAIN_WALL) {
        cellChangeHealth(cell, m_terrainInfo->terrainWallHp);
        cellChangePassable(cell, false);
        cellChangePassableFoot(cell, false);
    }
    else if (terrainType == TERRAIN_BLOOM) {
        cellChangeCredits(cell, -23);

        s_GameEvent event {
            .eventType = eGameEventType::GAME_EVENT_SPICE_BLOOM_SPAWNED,
            .data = CommonEvent {
                .entityType = eBuildType::SPECIAL,
                .atCell = cell
            }
        };
        m_interface->onNotifyGameEvent(event);
    }
}

void cMap::clearAllCells()
{
    for (int c = 0; c < getMaxCells(); c++) {
        cellInit(c);
    }
}

bool cMap::isVisible(int iCell, cPlayer *thePlayer)
{
    if (!thePlayer) return false;
    int playerId = thePlayer->getId();
    return isVisible(iCell, playerId);
}

void cMap::setVisibleFor(int iCell, cPlayer *pPlayer)
{
    if (!pPlayer) return;
    setVisible(iCell, pPlayer->getId(), true);
}

void cMap::setVisible(int iCell, int iPlayer, bool flag)
{
    if (!isValidCell(iCell)) return;
    if (iPlayer < 0 || iPlayer >= MAX_PLAYERS) return;

    m_cell[iCell].iVisible[iPlayer] = flag;
}

int cMap::findNearestSpiceBloom(int iCell)
{
    int quarterOfMap = getWidth() / 4;
    int iDistance = quarterOfMap;
    int halfWidth = getWidth() / 2;
    int halfHeight = getHeight() / 2;

    if (iCell < 0) {
        // use cell at center
        iCell = getGeometry().getCellWithMapDimensions(halfWidth, halfHeight);
        iDistance = getWidth();
    }

    int cx, cy;
    int closestBloomFoundSoFar = -1;
    int bloomsEvaluated = 0;

    cx = m_mapGeometry->getCellX(iCell);
    cy = m_mapGeometry->getCellY(iCell);

    for (int i = 0; i < getMaxCells(); i++) {
        int cellType = getCellType(i);
        if (cellType != TERRAIN_BLOOM) continue;
        bloomsEvaluated++;

        int d = ABS_length(cx, cy, m_mapGeometry->getCellX(i), m_mapGeometry->getCellY(i));

        if (d < iDistance) {
            closestBloomFoundSoFar = i;
            iDistance = d;
        }
    }

    // found a close spice bloom
    if (closestBloomFoundSoFar > 0) {
        return closestBloomFoundSoFar;
    }

    // no spice blooms evaluated, abort
    if (bloomsEvaluated < 0) {
        return -1;
    }

    // randomly pick one
    int iTargets[10];
    memset(iTargets, -1, sizeof(iTargets));
    int iT = 0;

    for (int i = 0; i < getMaxCells(); i++) {
        int cellType = getCellType(i);
        if (cellType == TERRAIN_BLOOM) {
            iTargets[iT] = i;
            iT++;
            if (iT >= 10) break;
        }
    }

    // when finished, return bloom
    return iTargets[RNG::rnd(iT)];
}

bool cMap::isValidTerrainForStructureAtCell(int cll)
{
    if (!isValidCell(cll)) return false;
    int cellType = getCellType(cll);
    // TODO: make this a flag in the cell/terrain type kind? (there is no such thing yet, its all hard-coded)
    bool allowedCellType = (cellType == TERRAIN_SLAB || cellType == TERRAIN_ROCK);
    if (!allowedCellType) {
        return false;
    }

    return true;
}

/**
 * returns a (randomized) cell. Taking cell param as 'start' position and 'distance' the amount of cells you want to
 * move away. This function will not chose a random position between cell and position. Hence, the amount of cells
 * to move is guaranteed to be so many cells away.
 * @param cell
 * @param distance
 */
/**
 * Takes structure, evaluates all its cells, and if any of these are visible, this function returns true.
 *
 * @param pStructure
 * @param thePlayer (for who it should be visible)
 * @return
 */
bool cMap::isStructureVisible(cAbstractStructure *pStructure, cPlayer *thePlayer)
{
    if (!thePlayer) return false;
    return isStructureVisible(pStructure, thePlayer->getId());
}

/**
 * Takes structure, evaluates all its cells, and if any of these are visible, this function returns true.
 *
 * @param pStructure
 * @param iPlayer
 * @return
 */
bool cMap::isStructureVisible(cAbstractStructure *pStructure, int iPlayer)
{
    if (!pStructure) return false;
    if (!pStructure->isValid()) return false;

    // iterate over all cells of structure
    const std::vector<int> &cells = pStructure->getCellsOfStructure();
    for (auto &cll: cells) {
        if (isVisible(cll, iPlayer)) {
            return true;
        }
    }

    return false;
}

int cMap::findNearByValidDropLocation(int cell, int minRange, int range, int unitTypeToDrop)
{
    auto *mapCamera = m_interface->getMapCamera();

    if (minRange < 1) {
        minRange = 1;
    }

    // go around 360 fDegrees and calculate new stuff.
    for (float dr = minRange; dr < range; dr++) { // go outwards
        for (float d = 0; d < 360; d++) { // if we reduce the amount of degrees, we don't get full coverage.
            // need a smarter way to do this (less CPU intensive).

            int x = m_mapGeometry->getAbsoluteXPositionFromCellCentered(cell);
            int y = m_mapGeometry->getAbsoluteYPositionFromCellCentered(cell);

            float dr1 = cos(d) * (dr * TILESIZE_WIDTH_PIXELS);
            float dr2 = sin(d) * (dr * TILESIZE_HEIGHT_PIXELS);

            x = (x + dr1);
            y = (y + dr2);

            // convert back
            int cl = mapCamera->getCellFromAbsolutePosition(x, y);

            if (cl < 0) continue;
            if (!m_mapGeometry->isWithinBoundaries(cl)) continue;

            if (canDeployUnitTypeAtCell(cl, unitTypeToDrop)) {
                return cl;
            }
        }
    }
    return -1;
}

int cMap::findNearByValidDropLocation(int cell, int range, int unitTypeToDrop)
{
    return findNearByValidDropLocation(cell, 1, range, unitTypeToDrop);
}

// int cMap::findNearByValidDropLocationForUnit(int cell, int range, int unitIDToDrop)
// {
//     auto *mapCamera = m_interface->getMapCamera();

//     // go around 360 fDegrees and calculate new stuff.
//     for (float dr = 1; dr < range; dr++) { // go outwards
//         for (float d = 0; d < 360; d++) { // if we reduce the amount of degrees, we don't get full coverage.
//             // need a smarter way to do this (less CPU intensive).

//             int x = getAbsoluteXPositionFromCellCentered(cell);
//             int y = getAbsoluteYPositionFromCellCentered(cell);

//             float dr1 = cos(d) * (dr * TILESIZE_WIDTH_PIXELS);
//             float dr2 = sin(d) * (dr * TILESIZE_HEIGHT_PIXELS);

//             x = (x + dr1);
//             y = (y + dr2);

//             // convert back
//             int cl = mapCamera->getCellFromAbsolutePosition(x, y);

//             if (cl < 0) continue;

//             if (canDeployUnitAtCell(cell, unitIDToDrop)) {
//                 return cell;
//             }
//         }
//     }
//     return -1;
// }

/**
 * Scans structures, belonging to player. If it is the same type it will be returned.
 * @param player
 * @param structureType
 * @return
 */
cAbstractStructure *cMap::findClosestStructureType(int cell, int structureType, cPlayer *player)
{
    assert(player);
    assert(structureType > -1);
    assert(isValidCell(cell));

    int foundStructureId = -1;    // found structure id
    long shortestDistance = 9999; // max distance to search in

    const std::vector<int> &myStructuresAsId = player->getAllMyStructuresAsId();
    for (auto &i: myStructuresAsId) {
        cAbstractStructure *pStructure = m_objects->getStructure(i);
        if (pStructure == nullptr) continue;
        if (pStructure->getType() != structureType) continue;

        long _distance = m_mapGeometry->distance(cell, pStructure->getCell());

        // if distance is lower than last found distance, it is the closest for now.
        if (_distance < shortestDistance) {
            foundStructureId = i;
            shortestDistance = _distance;
        }
    }

    if (foundStructureId > -1) {
        return m_objects->getStructure(foundStructureId);
    }

    return nullptr;
}

void cMap::cellTakeDamage(int cellNr, int damage)
{
    tCell *pCell = getCell(cellNr);
    if (pCell) {
        pCell->health -= damage;

        if (pCell->health < -25) {
            if (pCell->type == TERRAIN_ROCK) {
                smudge_increase(SmudgeType::S_ROCK, cellNr);
            }

            if (pCell->type == TERRAIN_SAND ||
                    pCell->type == TERRAIN_HILL ||
                    pCell->type == TERRAIN_SPICE ||
                    pCell->type == TERRAIN_SPICEHILL) {
                smudge_increase(SmudgeType::S_SAND, cellNr);
            }

            pCell->health += RNG::rnd(35);
        }
    }
}

void cMap::cellChangeType(int cellNr, int type)
{
    tCell *pCell = getCell(cellNr);
    if (pCell) {
        if (type > TERRAIN_WALL) {
            pCell->type = type;
        }
        else if (type < TERRAIN_BLOOM) {
            pCell->type = type;
        }
        else {
            pCell->type = type;
        }
    }
}

void cMap::cellInit(int cellNr)
{
    tCell *pCell = getCell(cellNr);
    if (!pCell) return; // bail

    pCell->credits = 0;
    pCell->health = 0;
    pCell->passable = true;
    pCell->passableFoot = true;
    pCell->tile = 0;
    pCell->type = TERRAIN_SAND;    // refers to gfxdata!

    pCell->smudgetile = -1;
    pCell->smudgetype = std::nullopt;

    // clear out the ID stuff
    memset(pCell->id, -1, sizeof(pCell->id));
    memset(pCell->iVisible, 0, sizeof(pCell->iVisible));
}

cAbstractStructure *cMap::findClosestAvailableStructureType(int cell, int structureType, cPlayer *pPlayer)
{
    assert(pPlayer);
    assert(structureType > -1);
    assert(isValidCell(cell));

    int foundStructureId = -1;    // found structure id
    long shortestDistance = 9999; // max distance to search in

    const std::vector<int> &myStructuresAsId = pPlayer->getAllMyStructuresAsId();
    for (auto &i: myStructuresAsId) {
        cAbstractStructure *pStructure = m_objects->getStructure(i);
        if (pStructure == nullptr) continue;
        if (pStructure->getType() != structureType) continue;
        if (pStructure->hasUnitWithin()) continue; // already occupied

        long _distance = m_mapGeometry->distance(cell, pStructure->getCell());

        // if distance is lower than last found distance, it is the closest for now.
        if (_distance < shortestDistance) {
            foundStructureId = i;
            shortestDistance = _distance;
        }
    }

    if (foundStructureId > -1) {
        return m_objects->getStructure(foundStructureId);
    }

    return nullptr;
}

cAbstractStructure *
cMap::findClosestAvailableStructureTypeWhereNoUnitIsHeadingTo(int cell, int structureType, cPlayer *pPlayer)
{
    assert(pPlayer);
    assert(structureType > -1);
    assert(isValidCell(cell));

    int foundStructureId = -1;    // found structure id
    long shortestDistance = 9999; // max distance to search in

    int playerId = pPlayer->getId();

    const std::vector<int> &myStructuresAsId = pPlayer->getAllMyStructuresAsId();
    for (auto &i: myStructuresAsId) {
        cAbstractStructure *pStructure = m_objects->getStructure(i);
        if (pStructure == nullptr) continue;
        if (pStructure->getOwner() != playerId) continue;
        if (pStructure->getType() != structureType) continue;
        if (pStructure->hasUnitWithin()) continue; // already occupied

        if (!pStructure->hasUnitHeadingTowards()) {    // no other unit is heading to this structure
            long _distance = m_mapGeometry->distance(cell, pStructure->getCell());

            // if distance is lower than last found distance, it is the closest for now.
            if (_distance < shortestDistance) {
                foundStructureId = i;
                shortestDistance = _distance;
            }
        }
    }

    if (foundStructureId > -1) {
        return m_objects->getStructure(foundStructureId);
    }

    return nullptr;
}

int cMap::getRandomCellFromWithRandomDistanceValidForUnitType(int cell, int minRange, int maxRange, int unitType)
{
    return findNearByValidDropLocation(cell, minRange, maxRange, unitType);
}

int cMap::findRandomCellToMoveToForSandworm() {
    for (int iTries = 0; iTries < 5; iTries++) {
        int iMoveTo = m_mapGeometry->getRandomCellWithinMapWithSafeDistanceFromBorder(2);
        if (isCellPassableForWorm(iMoveTo)) {
            return iMoveTo;
        }
    }
    return -1;
}

bool cMap::isCellPassableForWorm(int cell)
{
    if (cell < 0) return false;
    int cellType = getCellType(cell);
    return cellType == TERRAIN_SAND ||
           cellType == TERRAIN_HILL ||
           cellType == TERRAIN_SPICE ||
           cellType == TERRAIN_SPICEHILL;
}

bool cMap::isValidTerrainForConcreteAtCell(int cell)
{
    if (!isValidCell(cell)) return false;
    int cellType = getCellType(cell);
    // TODO: make this a flag in the cell/terrain type kind? (there is no such thing yet, its all hard-coded)
    bool allowedCellType = (cellType == TERRAIN_SLAB || cellType == TERRAIN_ROCK);
    if (!allowedCellType) {
        return false;
    }

    return true;
}

void cMap::detonateSpiceBloom(int cell)
{
    if (!isValidCell(cell)) return;
    int cellTypeAtCell = getCellType(cell);
    if (cellTypeAtCell != TERRAIN_BLOOM) return;

    // change type of terrain to sand
    auto mapEditor = cMapEditor(*this);
    mapEditor.createCell(cell, TERRAIN_SAND, 0);
    int size = 75 + (RNG::rnd(100));
    mapEditor.createRandomField(cell, TERRAIN_SPICE, size);
    m_interface->shakeScreen(20);

    s_GameEvent event {
        .eventType = eGameEventType::GAME_EVENT_SPICE_BLOOM_BLEW,
        .data = CommonEvent {
            .entityType = eBuildType::SPECIAL,
            .atCell = cell,
        }
    };
    m_interface->onNotifyGameEvent(event);

}

void cMap::onNotifyGameEvent(const s_GameEvent &event)
{
    switch (event.eventType) {
        case eGameEventType::GAME_EVENT_SPICE_BLOOM_SPAWNED:
            if (m_bAutoDetonateSpiceBlooms) {
                if (const auto *commonEvent = std::get_if<CommonEvent>(&event.data)) {
                    // 1000/5 = taking care of thinkFast 5ms loop. The second part
                    // is the actual amount of seconds we want to delay before blowing up the spice bloom
                    m_mBloomTimers[commonEvent->atCell] = (1000 / 5) * (45 + RNG::rnd(120));
                }
            }
            break;
        case eGameEventType::GAME_EVENT_SPICE_BLOOM_BLEW:
            if (m_bAutoDetonateSpiceBlooms) {
                if (const auto *commonEvent = std::get_if<CommonEvent>(&event.data)) {
                    m_mBloomTimers.erase(commonEvent->atCell);
                }
            }
            break;
        case eGameEventType::GAME_EVENT_DESTROYED:
            onEntityDestroyed(event);
            break;
        case eGameEventType::GAME_EVENT_CREATED:
            onEntityCreated(event);
            break;
        default:
            break;
    }
}

void cMap::onEntityCreated(const s_GameEvent &event)
{
    if (const auto *commonEvent = std::get_if<CommonEvent>(&event.data)) {
        if (commonEvent->entityType != eBuildType::UNIT) return;

        // only care about units
        if (commonEvent->entitySpecificType == SANDWORM) {
            evaluateIfWeShouldSetTimerToRespawnWorm();
        }
    }
}

void cMap::evaluateIfWeShouldSetTimerToRespawnWorm()
{
    int currentAmountOfWorms = m_objects->getPlayer(AI_WORM)->getAmountOfUnitsForType(SANDWORM);

    // as long as we don't have the desired amount, set respawn timer
    if (currentAmountOfWorms < m_iDesiredAmountOfWorms) {
        setSandwormRespawnTimer();
    }
    else {
        // we spawned a worm and got to the total amount, so set timer to -1
        // until a worm has been destroyed (either by enemy units or by withdrawal of worm)
        m_iTIMER_respawnSandworms = -1;
        cLogger::getInstance()->log(LOG_DEBUG, COMP_STRUCTURES, "evaluateIfWeShouldSetTimerToRespawnWorm", 
            std::format("set m_iTIMER_respawnSandworms to -1, because current amount sandworms ({}) == desired amount ({})",
                currentAmountOfWorms, m_iDesiredAmountOfWorms));
    }
}

void cMap::setSandwormRespawnTimer()
{
    //only set timer when it has been < 0 (not set already)
    if (m_iTIMER_respawnSandworms < 0) {
        // 1000/5 = taking care of thinkFast 5ms loop. The second part
        // is the actual amount of seconds we want to delay

        // give at least a minute (max 3) without that sandworm
        m_iTIMER_respawnSandworms = (1000 / 5) * (60 + RNG::rnd(180));

        cLogger::getInstance()->log(LOG_DEBUG, COMP_UNITS, "setSandwormRespawnTimer",  
            std::format("set timer to {}", this->m_iTIMER_respawnSandworms));
    }
    else {
        cLogger::getInstance()->log(LOG_DEBUG, COMP_UNITS, "setSandwormRespawnTimer",
            std::format("not change value because timer was already set (value = {})",
                    this->m_iTIMER_respawnSandworms));
    }
}

void cMap::setDesiredAmountOfWorms(int value)
{
    cLogger::getInstance()->log(LOG_DEBUG, COMP_UNITS, "setDesiredAmountOfWorms",
        std::format("changed value from {} to {}", this->m_iDesiredAmountOfWorms, value));
    m_iDesiredAmountOfWorms = value;
    evaluateIfWeShouldSetTimerToRespawnWorm();
}

void cMap::onEntityDestroyed(const s_GameEvent &event)
{
    if (const auto *commonEvent = std::get_if<CommonEvent>(&event.data)) {
        if (commonEvent->entityType != eBuildType::UNIT) return;

        // only care about units
        if (commonEvent->entitySpecificType == SANDWORM) {
            // a sandworm got destroyed, set timer to re-spawn it
            setSandwormRespawnTimer();
        }
    }
}

