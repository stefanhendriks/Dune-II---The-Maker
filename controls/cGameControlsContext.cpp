#include "cGameControlsContext.h"

#include "../include/d2tmh.h"

cGameControlsContext::cGameControlsContext(cPlayer *thePlayer, cMouse *theMouse) {
    assert(thePlayer);
    player = thePlayer;
    mouseCell = -99;
    drawToolTip = false;
    mouseHoveringOverUnitId = -1;
    mouseHoveringOverStructureId = -1;
    state = MOUSESTATE_SELECT;
    prevState = MOUSESTATE_SELECT;
    prevStateBeforeRepair = MOUSESTATE_SELECT;
    mouse = theMouse;
    mouseNormalState = new cMouseNormalState(thePlayer, this, mouse);
    mouseUnitsSelectedState = new cMouseUnitsSelectedState(thePlayer, this, mouse);
    mouseRepairState = new cMouseRepairState(thePlayer, this, mouse);
    mousePlaceState = new cMousePlaceState(thePlayer, this, mouse);
    mouseDeployState = new cMouseDeployState(thePlayer, this, mouse);
    prevTickMouseAtBattleField = false;
}

cGameControlsContext::~cGameControlsContext() {
    player = nullptr;
    delete mouseNormalState;
    delete mouseUnitsSelectedState;
    delete mouseRepairState;
    delete mousePlaceState;
    delete mouseDeployState;
}


void cGameControlsContext::updateMouseCell(const cPoint &coords) {
    if (coords.y < cSideBar::TopBarHeight) {
        mouseCell = MOUSECELL_TOPBAR; // at the top bar or higher, so no mouse cell id.
        return;
    }

    if (drawManager->getMiniMapDrawer()->isMouseOver()) {
        mouseCell = MOUSECELL_MINIMAP; // on minimap
        return;
    }

    if (coords.x > (game.m_screenX - cSideBar::SidebarWidth)) {
        mouseCell = MOUSECELL_SIDEBAR; // on sidebar
        return;
    }

    mouseCell = getMouseCellFromScreen(coords.x, coords.y);
}

int cGameControlsContext::getMouseCellFromScreen(int mouseX, int mouseY) const {
    int absMapX = mapCamera->getAbsMapMouseX(mouseX);
    int absMapY = mapCamera->getAbsMapMouseY(mouseY);
    return mapCamera->getCellFromAbsolutePosition(absMapX, absMapY);
}

void cGameControlsContext::determineToolTip() {
    drawToolTip = false;
    if (key[KEY_T] && isMouseOnBattleField()) { // TODO: this gets removed later, when we redo tooltips anyway
        drawToolTip = true;
    }
}

void cGameControlsContext::determineHoveringOverStructureId() {
    mouseHoveringOverStructureId = -1;

    if (!map.isVisible(mouseCell, this->player)) {
        return; // cell not visible
    }

    mouseHoveringOverStructureId = map.getCellIdStructuresLayer(mouseCell);
}

void cGameControlsContext::determineHoveringOverUnitId() {
    if (mouseHoveringOverUnitId > -1) {
        cUnit &aUnit = unit[mouseHoveringOverUnitId];
        if (aUnit.isValid()) {
            aUnit.bHovered = false;
        }
    }
    mouseHoveringOverUnitId = -1;
    int mc = getMouseCell();
    tCell *cellOfMouse = map.getCell(mc);
    if (cellOfMouse == nullptr) return; // mouse is not on battlefield

    if (!map.isVisible(mc, this->player)) {
        return; // cell not visible
    }

    if (cellOfMouse->id[MAPID_UNITS] > -1) {
        int iUnitId = cellOfMouse->id[MAPID_UNITS];

        if (unit[iUnitId].iTempHitPoints < 0) {
            mouseHoveringOverUnitId = iUnitId;
        }

    } else if (cellOfMouse->id[MAPID_WORMS] > -1) {
        int iUnitId = cellOfMouse->id[MAPID_WORMS];
        mouseHoveringOverUnitId = iUnitId;
    }

    if (mouseHoveringOverUnitId > -1) {
        cUnit &aUnit = unit[mouseHoveringOverUnitId];
        if (aUnit.isValid()) {
            aUnit.bHovered = true;
        }
    }
}

cAbstractStructure *cGameControlsContext::getStructurePointerWhereMouseHovers() {
    if (mouseHoveringOverStructureId < 0) {
        return nullptr;
    }
    return structure[mouseHoveringOverStructureId];
}

void cGameControlsContext::onMouseMovedTo(const s_MouseEvent &event) {
    updateMouseCell(event.coords);
    bool mouseOnBattleField = isMouseOnBattleField();
    if (mouseOnBattleField) {
        determineToolTip();
        determineHoveringOverStructureId();
        determineHoveringOverUnitId();

        if (!prevTickMouseAtBattleField) {
            onFocusMouseStateEvent();
        }
    } else {
        mouseHoveringOverStructureId = -1;
        mouseHoveringOverUnitId = -1;

        if (prevTickMouseAtBattleField) {
            onBlurMouseStateEvent();
        }
    }

    // remember state
    prevTickMouseAtBattleField = mouseOnBattleField;
}

void cGameControlsContext::onNotifyMouseEvent(const s_MouseEvent &event) {
    if (event.eventType == eMouseEventType::MOUSE_MOVED_TO) {
        onMouseMovedTo(event);
    }

    // mouse states only apply to battlefield (for now)
    onNotifyMouseStateEvent(event);

    if (!isMouseOnBattleField()) {
        mouse->setTile(MOUSE_NORMAL);
    }

}

void cGameControlsContext::onNotifyMouseStateEvent(const s_MouseEvent &event) {
    if (isMouseOnBattleField()) {
        switch (state) {
            case MOUSESTATE_SELECT:
                mouseNormalState->onNotifyMouseEvent(event);
                break;
            case MOUSESTATE_UNITS_SELECTED:
                mouseUnitsSelectedState->onNotifyMouseEvent(event);
                break;
            case MOUSESTATE_REPAIR:
                mouseRepairState->onNotifyMouseEvent(event);
                break;
            case MOUSESTATE_PLACE:
                mousePlaceState->onNotifyMouseEvent(event);
                break;
            case MOUSESTATE_DEPLOY:
                mouseDeployState->onNotifyMouseEvent(event);
                break;
        }
    }
}

void cGameControlsContext::onNotifyKeyboardEvent(const cKeyboardEvent &event) {
    switch (state) {
        case MOUSESTATE_SELECT:
            mouseNormalState->onNotifyKeyboardEvent(event);
            break;
        case MOUSESTATE_UNITS_SELECTED:
            mouseUnitsSelectedState->onNotifyKeyboardEvent(event);
            break;
        case MOUSESTATE_REPAIR:
            mouseRepairState->onNotifyKeyboardEvent(event);
            break;
        case MOUSESTATE_PLACE:
            mousePlaceState->onNotifyKeyboardEvent(event);
            break;
        case MOUSESTATE_DEPLOY:
            mouseDeployState->onNotifyKeyboardEvent(event);
            break;
    }
}

void cGameControlsContext::setMouseState(eMouseState newState) {
    if (newState != state) {
        // Remember previous state as long as it is not the PLACE state, since we can't go back to that state
        if (state != eMouseState::MOUSESTATE_PLACE) {
            prevState = state;
            if (newState == eMouseState::MOUSESTATE_REPAIR) {
                prevStateBeforeRepair = state;
            }
        }

        // we might get into a situation where our previous state was repair state, and we get into
        // repair state again (ie, Repair->Place->Repair state). In that case, we override the 'prevState'
        // with the 'prevStateBeforeRepair'.
        // since we don't have a real 'history' or 'stack' of mouse states, this is for now good enough.
        if (newState == prevState && newState == eMouseState::MOUSESTATE_REPAIR) {
            prevState = prevStateBeforeRepair;
        }

        logbook(fmt::format("setMouseState() : changing state from [{}] to [{}] (prevState=[{}], prevStateBeforeRepair=[{}])", mouseStateString(state), mouseStateString(newState), mouseStateString(prevState), mouseStateString(prevStateBeforeRepair)));
        state = newState;
        switch (state) {
            case MOUSESTATE_SELECT:
                mouseNormalState->onStateSet();
                break;
            case MOUSESTATE_UNITS_SELECTED:
                mouseUnitsSelectedState->onStateSet();
                break;
            case MOUSESTATE_REPAIR:
                mouseRepairState->onStateSet();
                break;
            case MOUSESTATE_PLACE:
                mousePlaceState->onStateSet();
                break;
            case MOUSESTATE_DEPLOY:
                mouseDeployState->onStateSet();
                break;
        }
    }
}

void cGameControlsContext::toPreviousState() {
    setMouseState(prevState);
}

bool cGameControlsContext::isState(eMouseState other) {
    return this->state == other;
}

void cGameControlsContext::onBlurMouseStateEvent() {
    switch (state) {
        case MOUSESTATE_SELECT:
            mouseNormalState->onBlur();
            break;
        case MOUSESTATE_UNITS_SELECTED:
            mouseUnitsSelectedState->onBlur();
            break;
        case MOUSESTATE_REPAIR:
            mouseRepairState->onBlur();
            break;
        case MOUSESTATE_PLACE:
            mousePlaceState->onBlur();
            break;
        case MOUSESTATE_DEPLOY:
            mouseDeployState->onBlur();
            break;
    }
}

void cGameControlsContext::onFocusMouseStateEvent() {
    switch (state) {
        case MOUSESTATE_SELECT:
            mouseNormalState->onFocus();
            break;
        case MOUSESTATE_UNITS_SELECTED:
            mouseUnitsSelectedState->onFocus();
            break;
        case MOUSESTATE_REPAIR:
            mouseRepairState->onFocus();
            break;
        case MOUSESTATE_PLACE:
            mousePlaceState->onFocus();
            break;
        case MOUSESTATE_DEPLOY:
            mouseDeployState->onFocus();
            break;
    }
}

void cGameControlsContext::onNotifyGameEvent(const s_GameEvent &event) {
    switch (state) {
        case MOUSESTATE_SELECT:
            mouseNormalState->onNotifyGameEvent(event);
            break;
        case MOUSESTATE_UNITS_SELECTED:
            mouseUnitsSelectedState->onNotifyGameEvent(event);
            break;
        case MOUSESTATE_REPAIR:
            mouseRepairState->onNotifyGameEvent(event);
            break;
        case MOUSESTATE_PLACE:
            mousePlaceState->onNotifyGameEvent(event);
            break;
        case MOUSESTATE_DEPLOY:
            mouseDeployState->onNotifyGameEvent(event);
            break;
    }
}
