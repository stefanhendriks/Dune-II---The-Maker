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

//    cPlayer &player = players[HUMAN]; // TODO: get player interacting with?
//    cGameControlsContext *context = player.getGameControlsContext();
//    bool bOrderingUnits=false;
//
//    if (player.isNotPlacingSomething() && player.isNotDeployingSomething()) {
//        combat_mouse_normalCombatInteraction(player, bOrderingUnits, context->getMouseCell());
//    }
//
//    switch (event.eventType) {
//        case MOUSE_RIGHT_BUTTON_PRESSED:
//            onCombatMouseEventRightButtonClicked(event);
//            break;
//        case MOUSE_MOVED_TO:
//            onCombatMouseEventMovedTo(event);
//            break;
//        case MOUSE_LEFT_BUTTON_CLICKED:
//            if (context->isMouseOnBattleField()) {
//                onCombatMouseEventLeftButtonClicked(event);
//            }
//            break;
//        default:
//            // set to -1 only when it was > -1
//            mouse->resetDragViewportInteraction();
//            break;
//    }

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

//    const cPlayer *humanPlayer = &players[HUMAN];
//    const cGameControlsContext *pContext = humanPlayer->getGameControlsContext();
//    const int hoverUnitId = pContext->getIdOfUnitWhereMouseHovers();
//    const int hoverStructureId = pContext->getIdOfStructureWhereMouseHovers();
//
//    if (event.eventType == KEY_HOLD) {
//        if (event.key == KEY_R) {
//            // Mouse is hovering above a unit
//            if (hoverUnitId > -1) {
//                cUnit &hoverUnit = unit[hoverUnitId];
//                if (hoverUnit.iPlayer == HUMAN) {
//                    mouse->setTile(MOUSE_PICK);
//
//                    // wanting to repair this unit, check if it is allowed, if so, change mouse tile
//                    if (humanPlayer->hasAtleastOneStructure(REPAIR)) {
//                        if (hoverUnit.isDamaged() && !hoverUnit.isInfantryUnit() && !hoverUnit.isAirbornUnit()) {
//                            mouse->setTile(MOUSE_REPAIR);
//                        }
//                    }
//                }
//            } else if (hoverStructureId > -1) {
//                cAbstractStructure *pStructure = structure[hoverStructureId];
//                if (pStructure && pStructure->belongsTo(humanPlayer) && pStructure->isDamaged()) {
//                    mouse->setTile(MOUSE_REPAIR);
//                }
//            }
//        }
//    } else if (event.eventType == KEY_PRESSED) {
//        // also "released"
//        if (event.key == KEY_R) {
//            int mouseTile = getMouseTile(humanPlayer);
//            mouse->setTile(mouseTile);
//        }
//    }
}

void cGameControlsContext::setMouseState(eMouseState newState) {
    if (newState != state) {
        this->prevState = state;
        this->state = newState;
        char msg[255];
        sprintf(msg, "Changed mouseState from [%s] to [%s]", mouseStateString(prevState).c_str(), mouseStateString(state).c_str());
        logbook(msg);
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
