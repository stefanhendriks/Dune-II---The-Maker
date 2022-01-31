#include "cMouseNormalState.h"

#include "controls/cGameControlsContext.h"

#include "utils/cRectangle.h"
#include "utils/cSoundPlayer.h"
#include "gameobjects/units/cUnit.h"
#include "gameobjects/structures/cAbstractStructure.h"
#include "player/cPlayer.h"

#include "d2tmc.h"

#include "data/gfxdata.h"

#include <algorithm>

cMouseNormalState::cMouseNormalState(cPlayer *player, cGameControlsContext *context, cMouse *mouse) :
    cMouseState(player, context, mouse),
    state(SELECT_STATE_NORMAL) {
}

void cMouseNormalState::onNotifyMouseEvent(const s_MouseEvent &event) {

    // these methods can have a side-effect which changes mouseTile...
    switch (event.eventType) {
        case MOUSE_LEFT_BUTTON_PRESSED:
            mouse->boxSelectLogic(context->getMouseCell());
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
    if (context->isState(MOUSESTATE_SELECT)) { // if , required in case we switched state
        mouse->setTile(mouseTile);
    }
}

void cMouseNormalState::onMouseLeftButtonClicked() {
    bool selectedUnits = false;
    if (mouse->isBoxSelecting()) {
        player->deselectAllUnits();

        // remember, these are screen coordinates
        // TODO: Make it use absolute coordinates? (so we could have a rectangle bigger than the screen at one point?)
        cRectangle boxSelectRectangle = mouse->getBoxSelectRectangle();

        const std::vector<int> &ids = player->getAllMyUnitsWithinViewportRect(boxSelectRectangle);
        selectedUnits = player->selectUnits(ids);
    } else {
        bool infantrySelected = false;
        bool unitSelected = false;

        if (state == SELECT_STATE_NORMAL) {
            // single click, no box select
            int hoverUnitId = context->getIdOfUnitWhereMouseHovers();
            if (hoverUnitId > -1) {
                player->deselectAllUnits();

                cUnit &pUnit = unit[hoverUnitId];
                if (pUnit.isValid() && pUnit.belongsTo(player) && !pUnit.bSelected) {
                    pUnit.bSelected = true;
                    if (pUnit.isInfantryUnit()) {
                        infantrySelected = true;
                    } else {
                        unitSelected = true;
                    }
                }
            }

            int hoverStructureId = context->getIdOfStructureWhereMouseHovers();
            if (hoverStructureId > -1) {
                player->selected_structure = hoverStructureId;
                cAbstractStructure *pStructure = player->getSelectedStructure();
                if (pStructure && pStructure->isValid() && pStructure->belongsTo(player)) {
                    eListType listId = pStructure->getAssociatedListID();
                    if (listId != eListType::LIST_NONE) {
                        player->getSideBar()->setSelectedListId(listId);
                    }
                } else {
                    player->selected_structure = -1;
                }
            }
        } else if (state == SELECT_STATE_RALLY) {
            // setting a rally point!
            cAbstractStructure *pStructure = player->getSelectedStructure();
            if (pStructure && pStructure->isValid()) {
                pStructure->setRallyPoint(context->getMouseCell());
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
        context->setMouseState(MOUSESTATE_UNITS_SELECTED);
    }

    mouse->resetBoxSelect();
}

void cMouseNormalState::onMouseRightButtonPressed() {
    mouse->dragViewportInteraction();
}

void cMouseNormalState::onMouseRightButtonClicked() {
    mouse->resetDragViewportInteraction();
}

void cMouseNormalState::onMouseMovedTo() {
    if (state == SELECT_STATE_NORMAL) {
        mouseTile = getMouseTileForNormalState();
    } else if (state == SELECT_STATE_RALLY) {
        mouseTile = MOUSE_RALLY;
    }
}

int cMouseNormalState::getMouseTileForNormalState() const {
    int hoverUnitId = context->getIdOfUnitWhereMouseHovers();
    if (hoverUnitId > -1) {
        cUnit &pUnit = unit[hoverUnitId];
        if (pUnit.isValid() && pUnit.belongsTo(player)) {
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
    mouse->setTile(mouseTile);
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
    if (context->isState(MOUSESTATE_SELECT)) { // if , required in case we switched state
        mouse->setTile(mouseTile);
    }
}

void cMouseNormalState::onKeyDown(const cKeyboardEvent &event) {
    if (event.hasKey(KEY_LCONTROL) || event.hasKey(KEY_RCONTROL)) {
        cAbstractStructure *pSelectedStructure = player->getSelectedStructure();
        // when selecting a structure
        if (pSelectedStructure && pSelectedStructure->belongsTo(player) && pSelectedStructure->canSpawnUnits()) {
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
            player->deselectAllUnits();
            bool anyUnitSelected = player->selectUnitsFromGroup(iGroup);
            if (anyUnitSelected) {
                player->setContextMouseState(MOUSESTATE_UNITS_SELECTED);
            }
        }
    }

    if (event.hasKey(KEY_R)) {
        context->setMouseState(MOUSESTATE_REPAIR);
    }
}

void cMouseNormalState::setState(eMouseNormalState newState) {
    if (game.isDebugMode()) {
        char msg[255];
        sprintf(msg, "Setting state from %s to %s", mouseNormalStateString(state), mouseNormalStateString(newState));
        logbook(msg);
    }
    state = newState;
}

void cMouseNormalState::onFocus() {
    mouse->setTile(mouseTile);
}
