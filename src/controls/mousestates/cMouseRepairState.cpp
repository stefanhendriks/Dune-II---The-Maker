#include "cMouseRepairState.h"

#include "d2tmc.h"
#include "data/gfxdata.h"
#include "controls/cGameControlsContext.h"
#include "player/cPlayer.h"

#include <algorithm>

cMouseRepairState::cMouseRepairState(cPlayer *player, cGameControlsContext *context, cMouse *mouse) :
        cMouseState(player, context, mouse) {

}

void cMouseRepairState::onNotifyMouseEvent(const s_MouseEvent &event) {
    // these methods can have a side-effect which changes mouseTile...
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
    if (m_context->isState(MOUSESTATE_REPAIR)) { // if , required in case we switched state
        m_mouse->setTile(mouseTile);
    }
}

void cMouseRepairState::onMouseLeftButtonClicked() {
    int hoverUnitId = m_context->getIdOfUnitWhereMouseHovers();
    if (hoverUnitId > -1) {
        cUnit &pUnit = unit[hoverUnitId];
        if (pUnit.isValid() && pUnit.belongsTo(m_player) && pUnit.isEligibleForRepair()) {
            pUnit.findBestStructureCandidateAndHeadTowardsItOrWait(REPAIR, true, INTENT_REPAIR);
        }
    }

    cAbstractStructure *pStructure = m_context->getStructurePointerWhereMouseHovers();
    if (pStructure && pStructure->isValid()) {
        if (pStructure->belongsTo(m_player) && pStructure->isDamaged()) {
            pStructure->setRepairing(!pStructure->isRepairing());
        }
    }
}

void cMouseRepairState::onMouseRightButtonPressed() {
    m_mouse->dragViewportInteraction();
}

void cMouseRepairState::onMouseRightButtonClicked() {
    if (!m_mouse->isMapScrolling()) {
        m_context->toPreviousState();
    }

    m_mouse->resetDragViewportInteraction();
}

void cMouseRepairState::onMouseMovedTo() {
    mouseTile = getMouseTileForRepairState();
}

void cMouseRepairState::onStateSet() {
    mouseTile = getMouseTileForRepairState();
    m_mouse->setTile(mouseTile);
}


void cMouseRepairState::onNotifyKeyboardEvent(const cKeyboardEvent &event) {
    if (event.isType(eKeyEventType::PRESSED) && event.hasKey(KEY_R)) {
        m_context->toPreviousState();
    }
}

int cMouseRepairState::getMouseTileForRepairState() {
    int hoverUnitId = m_context->getIdOfUnitWhereMouseHovers();
    if (hoverUnitId > -1) {
        cUnit &pUnit = unit[hoverUnitId];
        if (pUnit.isValid() && pUnit.belongsTo(m_player) && pUnit.isEligibleForRepair()) {
            return MOUSE_REPAIR;
        }
    }

    cAbstractStructure *pStructure = m_context->getStructurePointerWhereMouseHovers();
    if (pStructure && pStructure->isValid()) {
        if (pStructure->belongsTo(m_player) && pStructure->isDamaged()) {
            return MOUSE_REPAIR;
        }
    }

    return MOUSE_FORBIDDEN;
}

void cMouseRepairState::onFocus() {
    m_mouse->setTile(mouseTile);
}

void cMouseRepairState::onBlur() {
    m_mouse->resetBoxSelect();
    m_mouse->resetDragViewportInteraction();
}
