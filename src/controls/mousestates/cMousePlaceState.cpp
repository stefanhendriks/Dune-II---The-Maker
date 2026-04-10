#include "cMousePlaceState.h"

#include "game/cGame.h"
#include "include/d2tmc.h"
#include "map/cMap.h"
#include "data/gfxdata.h"
#include "controls/cGameControlsContext.h"
#include "gameobjects/structures/cStructures.h"
#include "gameobjects/units/cUnits.h"
#include "utils/cStructureUtils.h"
#include "player/cPlayer.h"
#include "sidebar/cSideBar.h"
#include "utils/cSoundPlayer.h"
#include "map/MapGeometry.hpp"
#include "context/cInfoContext.h"
#include "context/cGameObjectContext.h"
#include <algorithm>
#include <cassert>

#include "data/gfxaudio.h"

cMousePlaceState::cMousePlaceState(cPlayer *player, cGameControlsContext *context, cMouse *mouse) :
    cMouseState(player, context, mouse)
{
    assert(player!=nullptr);
    assert(context!=nullptr);
    assert(mouse!=nullptr);
}

void cMousePlaceState::onNotifyMouseEvent(const s_MouseEvent &event)
{
    // these methods can have a side-effect which changes m_mouseTile...
    switch (event.eventType) {
        case MOUSE_LEFT_BUTTON_CLICKED:
            onMouseLeftButtonClicked();
            break;
        case MOUSE_RIGHT_BUTTON_PRESSED:
            onMouseRightButtonPressed();
            break;
        case MOUSE_RIGHT_BUTTON_CLICKED:
            onMouseRightButtonClicked();
            break;
        case MOUSE_MOVED_TO:
            onMouseMovedTo();
            break;
        default:
            break;
    }

    // ... so set it here
    if (m_context->isState(MOUSESTATE_PLACE)) { // if , required in case we switched state
        m_mouse->setTile(m_mouseTile);
    }
}

void cMousePlaceState::onMouseLeftButtonClicked()
{
    // this assumes the m_context has been updated beforehand...
    int mouseCell = m_context->getMouseCell();

    if (mouseCell < 0) {
        return;
    }

    cBuildingListItem *itemToPlace = m_player->getSideBar()->getList(eListType::LIST_CONSTYARD)->getItemToPlace();
    if (itemToPlace == nullptr) {
        return;
    }

    if (mayPlaceIt(itemToPlace, m_context->getMouseCell())) {
        game.playSound(SOUND_PLACE);
        m_player->placeItem(mouseCell, itemToPlace);
        m_context->toPreviousState();
    }
}

bool cMousePlaceState::mayPlaceIt(cBuildingListItem *itemToPlace, int mouseCell)
{
    // TODO: Maybe reduce logic here by combining it with player->canPlaceXXXXXX function(s)?
    if (mouseCell < 0) {
        return false;
    }

    int structureIdToPlace = itemToPlace->getBuildId();
    assert(structureIdToPlace > -1);

    bool bWithinBuildDistance = false;

    int cellWidth = game.m_structureUtils->getWidthOfStructureTypeInCells(structureIdToPlace);
    int cellHeight = game.m_structureUtils->getHeightOfStructureTypeInCells(structureIdToPlace);

#define SCANWIDTH    1

    int iCellX = game.m_gameObjectsContext->getMap().getCellX(mouseCell);
    int iCellY = game.m_gameObjectsContext->getMap().getCellY(mouseCell);

    // check
    int iStartX = iCellX - SCANWIDTH;
    int iStartY = iCellY - SCANWIDTH;

    int iEndX = iCellX + SCANWIDTH + cellWidth;
    int iEndY = iCellY + SCANWIDTH + cellHeight;

    // Fix up the boundaries
    cPoint::split(iStartX, iStartY) = game.m_gameObjectsContext->getMap().fixCoordinatesToBeWithinMap(iStartX, iStartY);
    cPoint::split(iEndX, iEndY) = game.m_gameObjectsContext->getMap().fixCoordinatesToBeWithinMap(iEndX, iEndY);

    // Determine if structure to be placed is within build distance
    for (int iX = iStartX; iX < iEndX; iX++) {
        for (int iY = iStartY; iY < iEndY; iY++) {
            int iCll = game.m_gameObjectsContext->getMap().getGeometry().getCellWithMapDimensions(iX, iY);

            if (iCll > -1) {
                int idOfStructureAtCell = game.m_gameObjectsContext->getMap().getCellIdStructuresLayer(iCll);
                if (idOfStructureAtCell > -1) {
                    int iID = idOfStructureAtCell;

                    if (game.m_gameObjectsContext->getStructures()[iID]->getOwner() == HUMAN) {
                        bWithinBuildDistance = true; // connection!
                        break;
                    }

                    // TODO: Allow placement nearby allies?
                }

                if (game.m_gameObjectsContext->getMap().getCellType(iCll) == TERRAIN_WALL ||
                        game.m_gameObjectsContext->getMap().getCellType(iCll) == TERRAIN_SLAB) {
                    bWithinBuildDistance = true;
                    // TODO: here we should actually find out if the slab is ours or not??
                    break;
                }
            }
            else {
                return false;
            }
        }
    }

    if (!bWithinBuildDistance) {
        return false;
    }

    for (int iX = 0; iX < cellWidth; iX++) {
        for (int iY = 0; iY < cellHeight; iY++) {
            int cellX = iCellX + iX;
            int cellY = iCellY + iY;

            if (!game.m_gameObjectsContext->getMap().isWithinBoundaries(cellX, cellY)) {
                return false;
            }

            int iCll = game.m_gameObjectsContext->getMap().getGeometry().makeCell(cellX, cellY);

            // occupied by units or structures
            int idOfStructureAtCell = game.m_gameObjectsContext->getMap().getCellIdStructuresLayer(iCll);
            if (idOfStructureAtCell > -1) {
                // may not place when we're not placing a slab.. hack hack
                if (structureIdToPlace != SLAB4) {
                    return false;
                }
            }

            // non slab 'structures' can be blocked by units
            if (structureIdToPlace != SLAB4 && structureIdToPlace != SLAB1) {
                int unitIdOnMap = game.m_gameObjectsContext->getMap().getCellIdUnitLayer(iCll);
                if (unitIdOnMap > -1) {
                    // temporarily dead units do not block, but alive units (non-dead) do block placement
                    if (!game.m_gameObjectsContext->getUnits()[unitIdOnMap].isDead()) {
                        return false;
                    }
                    // TODO: Allow placement, let units move aside when clicking before placement?
                }
            }
        }
    }

    return true;
}

void cMousePlaceState::onMouseRightButtonPressed()
{
    m_mouse->dragViewportInteraction();
}

void cMousePlaceState::onMouseRightButtonClicked()
{
    if (m_mouse->isMapScrolling()) {
        m_mouse->resetDragViewportInteraction();
    }
    else {
        m_context->toPreviousState();
    }
}

void cMousePlaceState::onMouseMovedTo()
{
    // TODO: update state for determining place result so that renderer does not need to do
    // that logic every frame (see cPlaceItDrawer)
}

void cMousePlaceState::onStateSet()
{
    m_mouseTile = MOUSE_NORMAL;
    m_mouse->setTile(m_mouseTile);
}


void cMousePlaceState::onNotifyKeyboardEvent(const cKeyboardEvent &)
{
}

void cMousePlaceState::onFocus()
{
    m_mouse->setTile(m_mouseTile);
}

void cMousePlaceState::onBlur()
{
    m_mouse->resetBoxSelect();
    m_mouse->resetDragViewportInteraction();
}
