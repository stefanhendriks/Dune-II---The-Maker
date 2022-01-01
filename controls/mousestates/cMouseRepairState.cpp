#include "cMouseRepairState.h"

#include <algorithm>
#include "d2tmh.h"

cMouseRepairState::cMouseRepairState(cPlayer *player, cGameControlsContext *context, cMouse *mouse) :
        cMouseState(player, context, mouse) {

}

void cMouseRepairState::onNotifyMouseEvent(const s_MouseEvent &event) {
    // these methods can have a side-effect which changes mouseTile...
    switch (event.eventType) {
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
    if (context->isState(MOUSESTATE_REPAIR)) { // if , required in case we switched state
        mouse->setTile(mouseTile);
    }
}

void cMouseRepairState::onMouseLeftButtonClicked(const s_MouseEvent &event) {
    int hoverUnitId = context->getIdOfUnitWhereMouseHovers();
    if (hoverUnitId > -1) {
        cUnit &pUnit = unit[hoverUnitId];
        if (pUnit.isValid() && pUnit.belongsTo(player) && pUnit.isEligibleForRepair()) {
            pUnit.findBestStructureCandidateAndHeadTowardsItOrWait(REPAIR, true);
        }
    }

    cAbstractStructure *pStructure = context->getStructurePointerWhereMouseHovers();
    if (pStructure && pStructure->isValid()) {
        if (pStructure->belongsTo(player) && pStructure->isDamaged()) {
            pStructure->setRepairing(!pStructure->isRepairing());
        }
    }
}

void cMouseRepairState::onMouseRightButtonPressed(const s_MouseEvent &event) {
    mouse->dragViewportInteraction();
}

void cMouseRepairState::onMouseRightButtonClicked(const s_MouseEvent &event) {
    if (!mouse->isMapScrolling()) {
        context->toPreviousState();
    }

    mouse->resetDragViewportInteraction();
}

void cMouseRepairState::onMouseMovedTo(const s_MouseEvent &event) {
    mouseTile = getMouseTileForRepairState();
}

void cMouseRepairState::onStateSet() {
    mouseTile = getMouseTileForRepairState();
    mouse->setTile(mouseTile);
}


void cMouseRepairState::onNotifyKeyboardEvent(const s_KeyboardEvent &event) {
    if (event.eventType == KEY_PRESSED && event.key == KEY_R) {
        context->toPreviousState();
    }
}

int cMouseRepairState::getMouseTileForRepairState() {
    int hoverUnitId = context->getIdOfUnitWhereMouseHovers();
    if (hoverUnitId > -1) {
        cUnit &pUnit = unit[hoverUnitId];
        if (pUnit.isValid() && pUnit.belongsTo(player) && pUnit.isEligibleForRepair()) {
            return MOUSE_REPAIR;
        }
    }

    cAbstractStructure *pStructure = context->getStructurePointerWhereMouseHovers();
    if (pStructure && pStructure->isValid()) {
        if (pStructure->belongsTo(player) && pStructure->isDamaged()) {
            return MOUSE_REPAIR;
        }
    }

    return MOUSE_FORBIDDEN;
}
