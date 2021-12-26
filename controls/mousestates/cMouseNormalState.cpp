#include <algorithm>
#include "d2tmh.h"

#include "cMouseNormalState.h"

cMouseNormalState::cMouseNormalState(cPlayer *player, cGameControlsContext *context, cMouse *mouse) : cMouseState(player, context, mouse) {

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
    mouse->setTile(mouseTile);
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
        auto position = std::find_if(ids.begin(), ids.end(), [&](const int & id){ return unit[id].isHarvester(); });
        bool hasHarvesterSelected = position != ids.end();

        position = std::find_if(ids.begin(), ids.end(), [&](const int & id){ return !unit[id].isHarvester() && !unit[id].isAirbornUnit(); });
        bool nonAirbornNonHarvesterUnitSelected = position != ids.end();

        if (hasHarvesterSelected && !nonAirbornNonHarvesterUnitSelected) {
            // select all the harvester units, skip airborn
            for (auto id : ids) {
                cUnit &pUnit = unit[id];
                if (pUnit.isAirbornUnit()) continue;
                if (!pUnit.isHarvester()) continue;
                pUnit.bSelected = true;
                unitSelected = true; // do it here, instead of iterating again
            }
        } else {
            // select all the non-harvester,non-airborn units
            for (auto id : ids) {
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
    if (mouse->isMapScrolling()){
        mouse->resetDragViewportInteraction();
    }

    // for now?
    player->deselectAllUnits();
}

void cMouseNormalState::onMouseMovedTo(const s_MouseEvent &event) {
    int hoverStructureId = context->getIdOfStructureWhereMouseHovers();
    int hoverUnitId = context->getIdOfUnitWhereMouseHovers();
    if (hoverStructureId > -1 || hoverUnitId > -1) {
        mouseTile = MOUSE_PICK;
    } else {
        mouseTile = MOUSE_NORMAL;
    }
}
