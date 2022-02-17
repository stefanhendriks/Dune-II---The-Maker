#include "cMouseNormalState.h"

#include "controls/cGameControlsContext.h"

#include "utils/cRectangle.h"
#include "utils/cSoundPlayer.h"
#include "gameobjects/units/cUnit.h"
#include "gameobjects/structures/cAbstractStructure.h"
#include "player/cPlayer.h"

#include "d2tmc.h"

#include "data/gfxdata.h"

#include <fmt/core.h>
#include <algorithm>

cMouseNormalState::cMouseNormalState(cPlayer *player, cGameControlsContext *context, cMouse *mouse) :
        cMouseState(player, context, mouse),
        m_state(SELECT_STATE_NORMAL) {
}

void cMouseNormalState::onNotifyMouseEvent(const s_MouseEvent &event) {

    // these methods can have a side-effect which changes mouseTile...
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
        m_mouse->setTile(mouseTile);
    }
}

void cMouseNormalState::onMouseLeftButtonClicked() {
    bool selectedUnits = false;
    if (m_mouse->isBoxSelecting()) {
        m_player->deselectAllUnits();

        // remember, these are screen coordinates
        // TODO: Make it use absolute coordinates? (so we could have a rectangle bigger than the screen at one point?)
        cRectangle boxSelectRectangle = m_mouse->getBoxSelectRectangle();

        const std::vector<int> &ids = m_player->getAllMyUnitsWithinViewportRect(boxSelectRectangle);
        selectedUnits = m_player->selectUnits(ids);
    } else {
        bool infantrySelected = false;
        bool unitSelected = false;

        if (m_state == SELECT_STATE_NORMAL) {
            // single click, no box select
            int hoverUnitId = m_context->getIdOfUnitWhereMouseHovers();
            if (hoverUnitId > -1) {
                m_player->deselectAllUnits();

                cUnit &pUnit = unit[hoverUnitId];
                if (pUnit.isValid() && pUnit.belongsTo(m_player) && !pUnit.bSelected) {
                    pUnit.bSelected = true;
                    if (pUnit.isInfantryUnit()) {
                        infantrySelected = true;
                    } else {
                        unitSelected = true;
                    }
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
                } else {
                    m_player->selected_structure = -1;
                }
            }
        } else if (m_state == SELECT_STATE_RALLY) {
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

void cMouseNormalState::onMouseRightButtonPressed() {
    m_mouse->dragViewportInteraction();
}

void cMouseNormalState::onMouseRightButtonClicked() {
    m_mouse->resetDragViewportInteraction();
}

void cMouseNormalState::onMouseMovedTo() {
    if (m_state == SELECT_STATE_NORMAL) {
        mouseTile = getMouseTileForNormalState();
    } else if (m_state == SELECT_STATE_RALLY) {
        mouseTile = MOUSE_RALLY;
    }
}

int cMouseNormalState::getMouseTileForNormalState() const {
    int hoverUnitId = m_context->getIdOfUnitWhereMouseHovers();
    if (hoverUnitId > -1) {
        cUnit &pUnit = unit[hoverUnitId];
        if (pUnit.isValid() && pUnit.belongsTo(m_player)) {
            // only show this for units
            return MOUSE_PICK;
        }
        // non-selectable units (all from other players), don't give a "pick" mouse tile
        return MOUSE_NORMAL;
    }
    return MOUSE_NORMAL;
}

void cMouseNormalState::onStateSet() {
    mouseTile = MOUSE_NORMAL;
    m_mouse->setTile(mouseTile);
}

void cMouseNormalState::onNotifyKeyboardEvent(const cKeyboardEvent &event) {
    // these methods can have a side-effect which changes mouseTile...
    switch (event.eventType) {
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
        m_mouse->setTile(mouseTile);
    }
}

void cMouseNormalState::onKeyDown(const cKeyboardEvent &event) {
    if (event.hasKey(KEY_LCONTROL) || event.hasKey(KEY_RCONTROL)) {
        cAbstractStructure *pSelectedStructure = m_player->getSelectedStructure();
        // when selecting a structure
        if (pSelectedStructure && pSelectedStructure->belongsTo(m_player) && pSelectedStructure->canSpawnUnits()) {
            setState(SELECT_STATE_RALLY);
            mouseTile = MOUSE_RALLY;
        }
    }
}

void cMouseNormalState::onKeyPressed(const cKeyboardEvent &event) {
    bool createGroup = event.hasKey(KEY_LCONTROL) || event.hasKey(KEY_RCONTROL);
    if (createGroup) {
        // actual group creation is at cGameLogic onKeyPressed
        setState(SELECT_STATE_NORMAL);
        mouseTile = MOUSE_NORMAL;
    } else {
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

    if (event.hasKey(KEY_R)) {
        m_context->setMouseState(MOUSESTATE_REPAIR);
    }
}

void cMouseNormalState::setState(eMouseNormalState newState) {
    if (game.isDebugMode()) {
        logbook(fmt::format("Setting state from {} to {}", mouseNormalStateString(m_state), mouseNormalStateString(newState)));
    }
    m_state = newState;
}

void cMouseNormalState::onFocus() {
    m_mouse->setTile(mouseTile);
}
