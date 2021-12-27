#include <algorithm>
#include "d2tmh.h"

#include "cMouseNormalState.h"

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
            onMouseLeftButtonClicked(event);
            break;
        case MOUSE_RIGHT_BUTTON_PRESSED:
            onMouseRightButtonPressed(event);
            break;
        case MOUSE_RIGHT_BUTTON_CLICKED:
            onMouseRightButtonClicked(event);
            break;
        case MOUSE_MOVED_TO:
            onMouseMovedTo(event);
            break;
        default:
            break;
    }

    // ... so set it here
    if (context->isState(MOUSESTATE_SELECT)) { // if , required in case we switched state
        mouse->setTile(mouseTile);
    }
}

cMouseNormalState::~cMouseNormalState() {

}

void cMouseNormalState::onMouseLeftButtonClicked(const s_MouseEvent &event) {
    bool infantrySelected = false;
    bool unitSelected = false;

    if (mouse->isBoxSelecting()) {
        player->deselectAllUnits();

        // remember, these are screen coordinates
        // TODO: Make it use absolute coordinates? (so we could have a rectangle bigger than the screen at one point?)
        cRectangle boxSelectRectangle = mouse->getBoxSelectRectangle();

        const std::vector<int> &ids = player->getAllMyUnitsWithinViewportRect(boxSelectRectangle);

        // check if there is a harvester in this group
        auto position = std::find_if(ids.begin(), ids.end(), [&](const int &id) { return unit[id].isHarvester(); });
        bool hasHarvesterSelected = position != ids.end();

        position = std::find_if(ids.begin(), ids.end(),
                                [&](const int &id) { return !unit[id].isHarvester() && !unit[id].isAirbornUnit(); });
        bool nonAirbornNonHarvesterUnitSelected = position != ids.end();

        if (hasHarvesterSelected && !nonAirbornNonHarvesterUnitSelected) {
            // select all the harvester units, skip airborn
            for (auto id: ids) {
                cUnit &pUnit = unit[id];
                if (pUnit.isAirbornUnit()) continue;
                if (!pUnit.isHarvester()) continue;
                pUnit.bSelected = true;
                unitSelected = true; // do it here, instead of iterating again
            }
        } else {
            // select all the non-harvester,non-airborn units
            for (auto id: ids) {
                cUnit &pUnit = unit[id];
                if (pUnit.isAirbornUnit()) continue;
                if (pUnit.isHarvester()) continue;
                pUnit.bSelected = true;
                if (pUnit.isInfantryUnit()) {
                    infantrySelected = true;
                } else {
                    unitSelected = true;
                }
            }
        }
    } else {
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
            }
        } else if (state == SELECT_STATE_RALLY) {
            // setting a rally point!
            cAbstractStructure *pStructure = player->getSelectedStructure();
            if (pStructure && pStructure->isValid()) {
                pStructure->setRallyPoint(context->getMouseCell());
            }
        }
    }


    // determine what kind of units are selected, and
//    std::vector<int> selectedUnitIds;
//    std::copy_if(ids.begin(), ids.end(), std::back_inserter(selectedUnitIds), [&](const int & id){ return unit[id].bSelected; });
//
//    for (auto id : selectedUnitIds) {
//
//    }

    if (unitSelected) {
        play_sound_id(SOUND_REPORTING);
    }

    if (infantrySelected) {
        play_sound_id(SOUND_YESSIR);
    }

    if (infantrySelected || unitSelected) {
        context->setMouseState(MOUSESTATE_UNITS_SELECTED);
    }

    mouse->resetBoxSelect();
}

void cMouseNormalState::onMouseRightButtonPressed(const s_MouseEvent &event) {
    mouse->dragViewportInteraction();
}

void cMouseNormalState::onMouseRightButtonClicked(const s_MouseEvent &event) {
    mouse->resetDragViewportInteraction();
}

void cMouseNormalState::onMouseMovedTo(const s_MouseEvent &event) {
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

void cMouseNormalState::onNotifyKeyboardEvent(const s_KeyboardEvent &event) {
    // these methods can have a side-effect which changes mouseTile...
    switch (event.eventType) {
        case KEY_HOLD:
            onKeyDown(event);
            break;
        case KEY_PRESSED:
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

void cMouseNormalState::onKeyDown(const s_KeyboardEvent &event) {
    if (event.key == KEY_LCONTROL || event.key == KEY_RCONTROL) {
        cAbstractStructure *pSelectedStructure = player->getSelectedStructure();
        // when selecting a structure
        if (pSelectedStructure && pSelectedStructure->belongsTo(player) && pSelectedStructure->canSpawnUnits()) {
            state = SELECT_STATE_RALLY;
            mouseTile = MOUSE_RALLY;
        }
    }
}

void cMouseNormalState::onKeyPressed(const s_KeyboardEvent &event) {
    if (event.key == KEY_LCONTROL || event.key == KEY_RCONTROL) {
        state = SELECT_STATE_NORMAL;
        mouseTile = MOUSE_NORMAL;
    }
    if (event.key == KEY_R) {
        context->setMouseState(MOUSESTATE_REPAIR);
    }
}
