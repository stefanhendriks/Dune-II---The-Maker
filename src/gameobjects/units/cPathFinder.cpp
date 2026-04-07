#include "gameobjects/units/cPathFinder.h"
#include "gameobjects/units/cUnit.h"
#include "game/cGame.h"
#include "data/gfxdata.h"
#include "include/d2tmc.h"
#include "utils/common.h"
#include "utils/RNG.hpp"
#include "map/MapGeometry.hpp"
#include "utils/d2tm_math.h"

// Initialize the static temp_map
ASTAR cPathFinder::temp_map[16384];

// Path creation definitions / var
#define CLOSED        -1
#define OPEN          0

/*
  Pathfinder

  Last revision: 19/02/2006

  The pathfinder will first eliminate any possibility that it will fail.
  It will check if the unit is free to move, if the timer is set correctly
  and so forth.

  Then the actual FDS path finder starts. Which will output a 'reversed' traceable
  path.

  Eventually this path is converted back to a waypoint string for units, and also
  optimized (skipping attaching cells which are reachable and closer to goal or further on
  path string).

  Return possibilities:
  -1 = FAIL (goalcell = cell, or cannot find path)
  -2 = Cannot move, all surrounded (blocked)
  -3 = Too many paths created
  -4 = Offset is not 0 (moving between cells)
  -99= iUnitId is < 0 (invalid input)
  */
int cPathFinder::createPath(int iUnitId, int iPathCountUnits)
{
    logbook("CREATE_PATH -- START");
    if (iUnitId < 0) {
        logbook("CREATE_PATH -- END 1");
        return -99; // Wut!?
    }

    cUnit &pUnit = game.getUnit(iUnitId);
    if (!pUnit.isValid() || pUnit.isDead()) {
        return -99; // for now...
    }

    // do not start calculating anything before we are on 0,0 x,y wise on a cell
    if (pUnit.isMovingBetweenCells()) {
        pUnit.log("CREATE_PATH -- END 2");
        return -4; // no calculation before we are straight on a cell
    }

    // Too many paths where created , so we wait a little.
    // make sure not to create too many paths at once
    if (game.m_pathsCreated > 40) {
        pUnit.log("CREATE_PATH -- END 3");
        pUnit.movewaitTimer.reset(50 + RNG::rnd(50));
        return -3;
    }

    int iCell = pUnit.getCell(); // current cell

    // When the goal == cell, then skip.
    if (iCell == pUnit.movement.iGoalCell) {
        pUnit.log("CREATE_PATH -- END 4");
        pUnit.log("ODD: The goal = cell?");
        return -1;
    }

    // when all around the unit there is no space, dont even bother
    if (pUnit.isUnableToMove()) {
        pUnit.log("CREATE_PATH -- END 5");
        pUnit.movewaitTimer.reset(30 + RNG::rnd(50));
        return -2;
    }

    // Now start create path
    // Clear unit path settings (index & path string)
    memset(pUnit.movement.iPath, -1, sizeof(pUnit.movement.iPath));
    pUnit.movement.iPathIndex = -1;

    // Search around a cell:
    int cx, cy, the_cll, ex, ey;
    int goal_cell = pUnit.movement.iGoalCell;
    int controller = pUnit.iPlayer;

    game.m_pathsCreated++;
    memset(temp_map, -1, sizeof(temp_map));

    the_cll = -1;
    ex = -1;
    ey = -1;
    cx = game.m_map.getCellX(iCell);
    cy = game.m_map.getCellY(iCell);

    // set very first... our start cell
    temp_map[iCell].cost = ABS_length(cx, cy, game.m_map.getCellX(goal_cell), game.m_map.getCellY(goal_cell));
    temp_map[iCell].parent = -1;
    temp_map[iCell].state = OPEN; // this one is opened by default

    bool valid = true;
    bool success = false;

    int sx, sy;
    double cost = -1;

    // WHILE VALID TO RUN THIS LOOP
    while (valid) {
        // iCell reached Goal Cell. We should have bailed out sooner.
        if (iCell == goal_cell) {
            valid = false;
            success = true;
            break;
        }

        int idOfStructureAtCell = game.m_map.cellGetIdFromLayer(iCell, MAPID_STRUCTURES);
        if (pUnit.iStructureID > -1) {
            if (idOfStructureAtCell == pUnit.iStructureID) {
                valid = false;
                success = true;
                pUnit.log("Found structure ID");
                break;
            }
        }

        if (pUnit.combat.iAttackStructure > -1) {
            if (idOfStructureAtCell == pUnit.combat.iAttackStructure) {
                valid = false;
                success = true;
                pUnit.log("Found attack structure ID");
                break;
            }
        }

        cx = game.m_map.getCellX(iCell);
        cy = game.m_map.getCellY(iCell);

        // starting position is cx-1 and cy-1
        sx = cx - 1;
        sy = cy - 1;

        // check for not going under zero
        ex = cx + 1;
        ey = cy + 1;

        // boundaries
        cPoint::split(sx, sy) = game.m_map.fixCoordinatesToBeWithinPlayableMap(sx, sy);
        cPoint::split(ex, ey) = game.m_map.fixCoordinatesToBeWithinPlayableMap(ex, ey);

//        if (ex <= cx)
//            pUnit.log("CX = EX");
//        if (ey <= cy)
//            pUnit.log("CY = EY");

        cost = 999999999;
        the_cll = -1;

        // go around the cell we are checking!
        bool bail_out = false;

        // circle around cell X wise
        for (cx = sx; cx <= ex; cx++) {
            // circle around cell Y wise
            for (cy = sy; cy <= ey; cy++) {
                // only check the 'cell' that is NOT the current cell.
                int cll = game.m_map.getGeometry().getCellWithMapBorders(cx, cy);

                // skip invalid cells
                if (cll < 0)
                    continue;

                // DO NOT CHECK SELF
                if (cll == iCell)
                    continue;

//                if (cll == startCell) {
//                    continue; // don't evaluate start position (in case you get around)
//                }

                // Determine if it is a good cell to use or not:
                bool good = false; // not good by default

                // not a sandworm
                int cellType = game.m_map.getCellType(cll);
                if (!pUnit.isSandworm()) {
                    // Step by step determine if its good
                    // 2 fases:
                    // 1 -> Occupation by unit/structures
                    // 2 -> Occupation by terrain (but only when it is visible, since we do not want to have an
                    //      advantage or some super intelligence by units for unknown territories!)
                    int idOfUnitAtCell = game.m_map.getCellIdUnitLayer(cll);
                    int idOfStructureAtCell = game.m_map.getCellIdStructuresLayer(cll);

                    if (idOfUnitAtCell == -1 && idOfStructureAtCell == -1) {
                        // there is nothing on this cell, that is good
                        good = true;
                    }

                    if (idOfStructureAtCell > -1) {
                        // when the cell is a structure, and it is the structure we want to attack, it is good

                        if (pUnit.combat.iAttackStructure > -1)
                            if (idOfStructureAtCell == pUnit.combat.iAttackStructure)
                                good = true;

                        if (pUnit.iStructureID > -1)
                            if (idOfStructureAtCell == pUnit.iStructureID)
                                good = true;

                    }

                    // blocked by other than our own unit
                    if (idOfUnitAtCell > -1) {
                        // occupied by a different unit than ourselves
                        if (idOfUnitAtCell != iUnitId) {
                            int iUID = idOfUnitAtCell;

                            if (iPathCountUnits != 0) {
                                if (iPathCountUnits <= 0) {
                                    // other units block our path
                                    good = false;
                                    pUnit.log("iPathCountUnits < 0 - variable 'good' becomes 'false'");
                                }
                            }

                            cUnit &unitAtCell = game.getUnit(iUID);
                            if (!unitAtCell.getPlayer()->isSameTeamAs(pUnit.getPlayer())) {
                                // allow running over enemy infantry/squishable units
                                if (unitAtCell.isInfantryUnit() &&
                                        pUnit.canSquishInfantry()) // and the current unit can squish
                                    good = true; // its infantry we want to run over, so don't be bothered!
                            }
                            // it is not good, other unit blocks
                        }
                        else {
                            good = true;
                        }
                    }

                    // is not visible, always good (since we don't know yet if its blocked!)
                    if (game.m_map.isVisible(cll, controller) == false) {
                        good = true;
                    }
                    else {
                        // walls stop us
                        if (cellType == TERRAIN_WALL) {
                            good = false;
                        }

                        // When we are infantry, we move through mountains. However, normal units do not
                        if (!pUnit.isInfantryUnit()) {
                            if (cellType == TERRAIN_MOUNTAIN) {
                                good = false;
                            }
                        }
                    }
                }
                else {
                    // Sandworm only cares about terrain type for good/bad cells
                    good = game.m_map.isCellPassableForWorm(cll);
                }

                if (!good) {
                    continue;
                }

//                pUnit.log(std::format("CREATE_PATH: cll [{}] != [{}] && temp_map[cll].state [{}] and good [{}]",
//                                      cll, iCell, temp_map[cll].state, good));

                // it is the goal cell
                if (cll == goal_cell) {
                    the_cll = cll;
                    cost = 0;
                    bail_out = true;
                    pUnit.log("CREATE_PATH: Found the goal cell, success, bailing out");
                    break;
                }

                bool isClosed = temp_map[cll].state == CLOSED;

                // when the cell (the attached one) is NOT the cell we are on and
                // the cell is CLOSED (not checked yet)
                if (cll != iCell &&         // not checking on our own
                        isClosed) {            // and is closed (else it's not valid to check)
                    int gcx = game.m_map.getCellX(goal_cell);
                    int gcy = game.m_map.getCellY(goal_cell);

                    // calculate the cost
                    int tempCost = temp_map[cll].cost;
                    double distanceCost = game.m_map.distance(cx, cy, gcx, gcy);
                    double newCost = distanceCost + tempCost;
//                        pUnit.log(std::format(
//                                "CREATE_PATH: tempCost [{}] + distanceCost [{}] = newCost = [{}] vs current cost [{}]",
//                                tempCost, distanceCost, newCost, cost));

                    // when the cost is lower than we had before
                    if (newCost < cost) {
                        // when the cost is lower than the previous cost, then we set the new cost and we set the cell
                        the_cll = cll;
                        cost = newCost;
                        // found a new cell, now decrease ipathcountunits
                        iPathCountUnits--;
//                            pUnit.log(std::format(
//                                    "CREATE_PATH: Waypoint found : cell {} - goalcell = {}, iPathCountUnits = {}", cll,
//                                    goal_cell, iPathCountUnits));
                    }
                    else {
//                            pUnit.log(std::format(
//                                    "CREATE_PATH: Waypoint found : cell {} - goalcell = {}, iPathCountUnits = {}", cll,
//                                    goal_cell, iPathCountUnits));
                    }
                } // END OF LOOP #2
            } // Y thingy

            // bail out
            if (bail_out) {
                pUnit.log("CREATE_PATH: BAIL");
                break;
            }

        } // X thingy

        // When found a new c(e)ll;
        if (the_cll > -1) {
            pUnit.log(std::format("Found cell as best candidate: {}, parent is {}", the_cll, iCell));
            // Open this one, so we do not check it again
            temp_map[the_cll].state = OPEN;
            temp_map[the_cll].parent = iCell;
            temp_map[the_cll].cost = cost;

            // int halfTile = 16;
            // int iPrevX = game.m_mapCamera->getWindowXPositionFromCellWithOffset(iCell, halfTile);
            // int iPrevY = game.m_mapCamera->getWindowYPositionFromCellWithOffset(iCell, halfTile);

            // int iDx = game.m_mapCamera->getWindowXPositionFromCellWithOffset(the_cll, halfTile);
            // int iDy = game.m_mapCamera->getWindowYPositionFromCellWithOffset(the_cll, halfTile);

            // if (game.m_drawUnitDebug) {
                // global_renderDrawer->renderLine( iPrevX, iPrevY, iDx, iDy, Color{0, 255, 0,255});
            // }

            // Now set c to the cll
            iCell = the_cll;
            if (iCell == goal_cell) {
                valid = false;
                success = true;
            }

        }
        else {
//            int prevCell = temp_map[iCell].parent;

//            if (prevCell > -1 ) {
//                int halfTile = 16;
//                int iPrevX = mapCamera->getWindowXPositionFromCellWithOffset(iCell, halfTile);
//                int iPrevY = mapCamera->getWindowYPositionFromCellWithOffset(iCell, halfTile);
//
//                int iDx = mapCamera->getWindowXPositionFromCellWithOffset(prevCell, halfTile);
//                int iDy = mapCamera->getWindowYPositionFromCellWithOffset(prevCell, halfTile);
//
//                _renderDrawer->renderLine(screen, iPrevX, iPrevY, iDx, iDy, Color{255, 0, 0));
//                pUnit.log(std::format("Failed to find new cell, backtracking. From {} back to {}", iCell, prevCell));
//                iCell = prevCell; // back track
//            } else {
            pUnit.log(std::format("Failed to find new cell, backtracking failed!"));
            valid = false;
            success = false;
            pUnit.log("FAILED TO CREATE PATH - nothing found to continue");
            break;
//            }
        }

    } // valid to run loop (and try to create a path)

    pUnit.log("CREATE_PATH -- valid loop finished");

    if (success) {
        pUnit.log("CREATE_PATH -- pathfinder got to goal-cell. Backtracing ideal path.");
        // read path!
        int temp_path[MAX_PATH_SIZE];

        memset(temp_path, -1, sizeof(temp_path));

        bool cp = true;

        int sc = iCell;
        int pi = 0;

        temp_path[pi] = sc;
        pi++;

        pUnit.log(std::format("Starting backtracing. Path index = {}, temp_path[0] = {}", pi, temp_path[pi]));

        // while we should create a path
        while (cp) {
            int tmp = temp_map[sc].parent;
            pUnit.log(std::format("sc = {} - temp_path[sc].parent = {}", sc, tmp));
            if (tmp > -1) {
                // found terminator (PARENT=CURRENT)
                if (tmp == sc) {
                    pUnit.log("found terminator, stop!");
                    cp = false;
                    continue;
                }
                else {
                    temp_path[pi] = tmp;
                    sc = temp_map[sc].parent;
                    pi++;
                    pUnit.log(std::format("Backtraced. Path index = {}, temp_path[0] = {}", pi, temp_path[pi]));
                }
            }
            else {
                cp = false;
            }

            if (pi >= MAX_PATH_SIZE)
                cp = false;

            if (sc == pUnit.getCell())
                cp = false;
        }

        // reverse
        int z = MAX_PATH_SIZE - 1;
        int a = 0;
        int iPrevCell = -1;

        while (z > -1) {
            if (temp_path[z] > -1) {
                // check if any other cell of temp_path also borders to the previous given cell, as that will save us time
                if (iPrevCell > -1) {
                    int iGoodZ = -1;

                    for (int sz = z; sz > 0; sz--) {
                        if (temp_path[sz] > -1) {

                            if (game.m_map.isCellAdjacentToOtherCell(iPrevCell, temp_path[sz])) {
                                iGoodZ = sz;
                            }
                            //if (ABS_length(iCellGiveX(iPrevCell), iCellGiveY(iPrevCell), iCellGiveX(temp_path[sz]), iCellGiveY(temp_path[sz])) <= 1)
                            //  iGoodZ=sz;
                        }
                        else
                            break;
                    }

                    if (iGoodZ < z && iGoodZ > -1)
                        z = iGoodZ;
                }

                pUnit.movement.iPath[a] = temp_path[z];
                iPrevCell = temp_path[z];
                a++;
            }
            z--;
        }

        // optimize path
        //nextcell=cell;
        pUnit.movement.iPathIndex = 1;

        // take the closest bordering cell as 'far' away to start with
        for (int i = 1; i < MAX_PATH_SIZE; i++) {
            int pathCell = pUnit.movement.iPath[i];
            if (pathCell > -1) {
                if (game.m_map.isCellAdjacentToOtherCell(pUnit.getCell(), pathCell)) {
                    pUnit.movement.iPathIndex = i;
                }
            }
        }

        // debug debug
        if (game.isDebugMode()) {
            for (int i = 0; i < MAX_PATH_SIZE; i++) {
                int pathCell = pUnit.movement.iPath[i];
                if (pathCell > -1) {
                    pUnit.log(std::format("WAYPOINT {} = {} ", i, pathCell));
                }
            }
        }

        pUnit.updateCellXAndY();
        pUnit.movement.bCalculateNewPath = false;


        //log("SUCCES");
        return 0; // success!

    }
    else {
        pUnit.log("CREATE_PATH -- not valid");

        // perhaps we can get closer when we DO take units into account?
        //path_id=-1;
    }

    pUnit.log("CREATE_PATH: Failed to create path!");
    return -1;
}


// find
int cPathFinder::returnCloseGoal(int iCll, int iMyCell, int iID)
{
    //
    int iSize = 1;
    int iStartX = game.m_map.getCellX(iCll) - iSize;
    int iStartY = game.m_map.getCellY(iCll) - iSize;
    int iEndX = game.m_map.getCellX(iCll) + iSize;
    int iEndY = game.m_map.getCellX(iCll) + iSize;

    float dDistance = 9999;

    int ix = game.m_map.getCellX(iMyCell);
    int iy = game.m_map.getCellY(iMyCell);

    bool bSearch = true;

    int iTheClosest = -1;

    while (bSearch) {
        iStartX = game.m_map.getCellX(iCll) - iSize;
        iStartY = game.m_map.getCellY(iCll) - iSize;
        iEndX = game.m_map.getCellX(iCll) + iSize;
        iEndY = game.m_map.getCellY(iCll) + iSize;

        // Fix boundaries
        cPoint::split(iStartX, iStartY) = game.m_map.fixCoordinatesToBeWithinPlayableMap(iStartX, iStartY);
        cPoint::split(iEndX, iEndY) = game.m_map.fixCoordinatesToBeWithinPlayableMap(iEndX, iEndY);

        // search
        for (int iSX = iStartX; iSX < iEndX; iSX++)
            for (int iSY = iStartY; iSY < iEndY; iSY++) {
                // find an empty cell
                int cll = game.m_map.getGeometry().getCellWithMapDimensions(iSX, iSY);

                float dDistance2 = ABS_length(iSX, iSY, ix, iy);

                int idOfStructureAtCell = game.m_map.getCellIdStructuresLayer(cll);
                int idOfUnitAtCell = game.m_map.getCellIdUnitLayer(cll);

                if ((idOfStructureAtCell < 0) && (idOfUnitAtCell < 0)) { // no unit or structure at cell
                    // depending on unit type, do not choose walls (or mountains)
                    int cellType = game.m_map.getCellType(cll);
                    if (game.m_infoContext->getUnitInfo(game.getUnit(iID).iType).infantry) {
                        if (cellType == TERRAIN_MOUNTAIN)
                            continue; // do not use this one
                    }

                    if (cellType == TERRAIN_WALL)
                        continue; // do not use this one

                    if (dDistance2 < dDistance) {
                        dDistance = dDistance2;
                        iTheClosest = cll;
                    }
                }
            }

        if (iTheClosest > -1)
            return iTheClosest;

        iSize++;

        if (iSize > 9) {
            bSearch = false;
            break; // get out
        }
    }

    // fail
    return iCll;
}
