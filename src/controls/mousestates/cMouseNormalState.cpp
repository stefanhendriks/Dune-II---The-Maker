#include "cMouseNormalState.h"

#include "controls/cGameControlsContext.h"
#include "controls/eKeyAction.h"

#include "utils/cRectangle.h"
#include "utils/cSoundPlayer.h"
#include "managers/cDrawManager.h"
#include "gameobjects/units/cUnit.h"
#include "gameobjects/structures/cAbstractStructure.h"
#include "player/cPlayer.h"

#include "game/cGame.h"
#include "include/d2tmc.h"

#include "utils/cLog.h"
#include "data/gfxdata.h"
#include "context/cInfoContext.h"
#include "context/cGameObjectContext.h"

#include <format>
#include <algorithm>
#include <cassert>

#include "data/gfxaudio.h"

cMouseNormalState::cMouseNormalState(cPlayer *player, cGameControlsContext *context, cMouse *mouse) :
    cMouseState(player, context, mouse),
    m_state(SELECT_STATE_NORMAL)
{
    assert(player!=nullptr);
    assert(context!=nullptr);
    assert(mouse!=nullptr);
}

void cMouseNormalState::onNotifyMouseEvent(const s_MouseEvent &event)
{

    // these methods can have a side-effect which changes m_mouseTile...
    switch (event.eventType) {
        case MOUSE_LEFT_BUTTON_PRESSED:
            m_mouse->boxSelectLogic(m_context->getMouseCell());
            break;
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
    if (m_context->isState(MOUSESTATE_SELECT)) { // if , required in case we switched state
        m_mouse->setTile(m_mouseTile);
    }
}

void cMouseNormalState::onMouseLeftButtonClicked()
{
    bool selectedUnits = false;
    if (m_mouse->isBoxSelecting()) {
        m_player->deselectAllUnits();

        // remember, these are screen coordinates
        // TODO: Make it use absolute coordinates? (so we could have a rectangle bigger than the screen at one point?)
        cRectangle boxSelectRectangle = m_mouse->getBoxSelectRectangle();

        const std::vector<int> &ids = m_player->getAllMyUnitsWithinViewportRect(boxSelectRectangle);
        selectedUnits = m_player->selectUnits(ids);

        if (!ids.empty()) {
            if (ids.size() > 1) {
                game.m_drawManager->setMessage(std::format("{} units selected", ids.size()));
            }
            else {
                cUnit *pUnit = game.m_gameObjectsContext->getUnit(ids[0]);
                game.m_drawManager->setMessage(pUnit->getUnitStatusForMessageBar());
            }
        }
    }
    else {
        bool infantrySelected = false;
        bool unitSelected = false;

        if (m_state == SELECT_STATE_NORMAL) {
            // single click, no box select
            int hoverUnitId = m_context->getIdOfUnitWhereMouseHovers();
            if (hoverUnitId > -1) {
                m_player->deselectAllUnits();

                cUnit *pUnit = game.m_gameObjectsContext->getUnit(hoverUnitId);
                if (pUnit->isValid() && pUnit->belongsTo(m_player) && !pUnit->isSelected()) {
                    pUnit->select();
                    if (pUnit->isInfantryUnit()) {
                        infantrySelected = true;
                    }
                    else {
                        unitSelected = true;
                    }
                    game.m_drawManager->setMessage(pUnit->getUnitStatusForMessageBar());
                }
            }

            int hoverStructureId = m_context->getIdOfStructureWhereMouseHovers();
            if (hoverStructureId > -1) {
                m_player->selected_structure = hoverStructureId;
                cAbstractStructure *pStructure = m_player->getSelectedStructure();
                if (pStructure && pStructure->isValid() && pStructure->belongsTo(m_player)) {
                    eListType listId = pStructure->getAssociatedListID();
                    if (listId != eListType::LIST_NONE) {
                        m_player->getSideBar()->setSelectedListId(listId);
                    }
                }
                else {
                    m_player->selected_structure = -1;
                }

                if (pStructure && pStructure->isValid()) {
                    game.m_drawManager->setMessage(pStructure->getStatusForMessageBar());
                }
            }
        }
        else if (m_state == SELECT_STATE_RALLY) {
            // setting a rally point!
            cAbstractStructure *pStructure = m_player->getSelectedStructure();
            if (pStructure && pStructure->isValid()) {
                pStructure->setRallyPoint(m_context->getMouseCell());
            }
        }

        if (unitSelected) {
            game.playSound(SOUND_REPORTING);
        }

        if (infantrySelected) {
            game.playSound(SOUND_YESSIR);
        }

        selectedUnits = unitSelected || infantrySelected;
    }

    if (selectedUnits) {
        m_context->setMouseState(MOUSESTATE_UNITS_SELECTED);
    }

    m_mouse->resetBoxSelect();
}

void cMouseNormalState::onMouseRightButtonPressed()
{
    m_mouse->dragViewportInteraction();
}

void cMouseNormalState::onMouseRightButtonClicked()
{
    m_mouse->resetDragViewportInteraction();
}

void cMouseNormalState::onMouseMovedTo()
{
    if (m_state == SELECT_STATE_NORMAL) {
        m_mouseTile = getMouseTileForNormalState();
    }
    else if (m_state == SELECT_STATE_RALLY) {
        m_mouseTile = MOUSE_RALLY;
    }
}

int cMouseNormalState::getMouseTileForNormalState() const
{
    int hoverUnitId = m_context->getIdOfUnitWhereMouseHovers();
    if (hoverUnitId > -1) {
        cUnit *pUnit = game.m_gameObjectsContext->getUnit(hoverUnitId);
        if (pUnit->isValid() && pUnit->belongsTo(m_player)) {
            // only show this for units
            return MOUSE_PICK;
        }
        // non-selectable units (all from other players), don't give a "pick" mouse tile
        return MOUSE_NORMAL;
    }
    return MOUSE_NORMAL;
}

void cMouseNormalState::onStateSet()
{
    m_mouseTile = MOUSE_NORMAL;
    m_mouse->setTile(m_mouseTile);
}

void cMouseNormalState::onNotifyKeyboardEvent(const cKeyboardEvent &event)
{
    // these methods can have a side-effect which changes m_mouseTile...
    switch (event.getType()) {
        case eKeyEventType::HOLD:
            onKeyDown(event);
            break;
        case eKeyEventType::PRESSED:
            onKeyPressed(event);
            break;
        default:
            break;
    }

    // ... so set it here
    if (m_context->isState(MOUSESTATE_SELECT)) { // if , required in case we switched state
        m_mouse->setTile(m_mouseTile);
    }
}

void cMouseNormalState::onKeyDown(const cKeyboardEvent &event)
{
    if (event.isAction(eKeyAction::ATTACK_MODE)) {
        cAbstractStructure *pSelectedStructure = m_player->getSelectedStructure();
        // when selecting a structure
        if (pSelectedStructure && pSelectedStructure->belongsTo(m_player) && pSelectedStructure->canSpawnUnits()) {
            setState(SELECT_STATE_RALLY);
            m_mouseTile = MOUSE_RALLY;
        }
    }
    if (event.isAction(eKeyAction::SELECT_INFANTRY_ON_MAP)) {
        const auto infantryUnits = m_player->getInfantryUnitsOnMap();
        m_player->selectUnits(infantryUnits);
        m_context->setMouseState(MOUSESTATE_UNITS_SELECTED);
    }

    if (event.isAction(eKeyAction::SELECT_INFANTRY_ON_SCREEN)) {
        const auto infantryUnits = m_player->getInfantryUnitsOnViewport(*game.m_mapViewport);
        m_player->selectUnits(infantryUnits);
        m_context->setMouseState(MOUSESTATE_UNITS_SELECTED);
    }
}

void cMouseNormalState::onKeyPressed(const cKeyboardEvent &event)
{
    if (event.isAction(eKeyAction::ATTACK_MODE)) {
        // actual group creation is at cGameLogic onKeyPressed
        setState(SELECT_STATE_NORMAL);
        m_mouseTile = MOUSE_NORMAL;
    }
    else {
        // select group
        int iGroup = event.getGroupNumber();

        if (iGroup > 0) {
            // select all units for group
            m_player->deselectAllUnits();
            bool anyUnitSelected = m_player->selectUnitsFromGroup(iGroup);
            if (anyUnitSelected) {
                m_player->setContextMouseState(MOUSESTATE_UNITS_SELECTED);
            }
        }
    }

    if (event.isAction(eKeyAction::REPAIR_UNIT) && m_player->getSelectedUnits().size() == 0) {
        m_context->setMouseState(MOUSESTATE_REPAIR);
    }
}

void cMouseNormalState::setState(eMouseNormalState newState)
{
    cLogger::getInstance()->log(LOG_DEBUG, COMP_GAMESTATE, "Setting state", 
                std::format("from {} to {}", mouseNormalStateString(m_state), mouseNormalStateString(newState)));
    m_state = newState;
}

void cMouseNormalState::onFocus()
{
    m_mouse->setTile(m_mouseTile);
}

void cMouseNormalState::onBlur()
{
    m_mouse->resetBoxSelect();
    m_mouse->resetDragViewportInteraction();
}
