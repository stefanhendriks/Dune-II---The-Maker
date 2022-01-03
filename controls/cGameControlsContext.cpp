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

    if (coords.x > (game.screen_x - cSideBar::SidebarWidth)) {
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
    if (key[KEY_T] && isMouseOnBattleField()) {
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
    if (isMouseOnBattleField()) {
        determineToolTip();
        determineHoveringOverStructureId();
        determineHoveringOverUnitId();
    } else {
        mouseHoveringOverStructureId = -1;
        mouseHoveringOverUnitId = -1;
    }

    // mouse went to battlefield
    if (!prevTickMouseAtBattleField && isMouseOnBattleField()) {
        // call 'onFocus' on state object
    }

    // mouse went from battlefield to non-battlefield segment on screen
    if (prevTickMouseAtBattleField && !isMouseOnBattleField()) {
        // call 'onBlur' on state object (lost focus)?
    }

    prevTickMouseAtBattleField = isMouseOnBattleField();

    // UPDATE MOUSE STATE
    updateMouseState();
}

void cGameControlsContext::onNotifyMouseEvent(const s_MouseEvent &event) {
    if (event.eventType == eMouseEventType::MOUSE_MOVED_TO) {
        onMouseMovedTo(event);
    }

    if (isMouseOnBattleField()) {
        // mouse states only apply to battlefield (for now)
        onNotifyMouseStateEvent(event);
    } else {
        // ...
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
//
//    if (bOrderingUnits) {
//        player.deselectStructure();
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
        }
    }
}

void cGameControlsContext::onNotifyKeyboardEvent(const s_KeyboardEvent &event) {
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

void cGameControlsContext::updateMouseState() {
//    const cGameControlsContext *context = pPlayer->getGameControlsContext();
//    int mc = context->getMouseCell();
//    if (mc > -1) {
//        if (!mouse->isTile(MOUSE_NORMAL) && !mouse->isTile(MOUSE_REPAIR)) {
//            // make sure to keep checking if this is still valid, ie, when state changed?
//            mouse->setTile(getMouseTile(pPlayer));
//        }
//    }

//    cPlayer *humanPlayer = &players[HUMAN];
//    int mouseTile = getMouseTile(humanPlayer);
//    mouse->setTile(mouseTile);
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
    }
}

//void
//cGame::combat_mouse_normalCombatInteraction(cPlayer &humanPlayer, bool &bOrderingUnits, int mouseCell) const {
//    // when mouse hovers above a valid cell
//    if (mouseCell > -1) {
//        mouseOnBattlefield(mouseCell, bOrderingUnits);
//    }
//
//    if (mouse->isLeftButtonPressed()) {
//        mouse->boxSelectLogic(mouseCell);
//    } else {
//        // where we box selecting? then this must be the unpress of the mouse button and thus we
//        // should start selecting units within the rectangle
//        if (mouse->isBoxSelecting()) {
//            cRectangle boxSelectRectangle = mouse->getBoxSelectRectangle();
//
//            //  char msg[256];
//            //  sprintf(msg, "MINX=%d, MAXX=%d, MINY=%d, MAXY=%d", min_x, min_y, max_x, max_y);
//            //  logbook(msg);
//
//            // Now do it!
//            // deselect all units
//            UNIT_deselect_all();
//            mouse->setTile(MOUSE_NORMAL);
//
//            bool bPlayRep = false;
//            bool bPlayInf = false;
//
//            bool harvesterSelected=false;
//            bool attackingUnitSelected=false;
//
//            for (int i = 0; i < MAX_UNITS; i++) {
//                cUnit &cUnit = unit[i];
//                if (!cUnit.isValid()) continue;
//                if (cUnit.iPlayer != HUMAN) continue;
//                // do not select airborn units
//                if (cUnit.isAirbornUnit()) {
//                    // always deselect unit:
//                    cUnit.bSelected = false;
//                    continue;
//                }
//                if (cUnit.iTempHitPoints >= 0) continue; // skip units that have been 'hidden' in a refinery or repair
//
//                if (boxSelectRectangle.isPointWithin(cUnit.center_draw_x(), cUnit.center_draw_y())) {
//                    // It is in the borders, select it
//                    cUnit.bSelected = true;
//
//                    if (cUnit.iType == HARVESTER) {
//                        harvesterSelected = true;
//                    } else {
//                        attackingUnitSelected = true;
//                    }
//
//                    if (sUnitInfo[cUnit.iType].infantry) {
//                        bPlayInf = true;
//                    } else {
//                        bPlayRep = true;
//                    }
//
//                }
//            }
//
//            if (harvesterSelected && attackingUnitSelected) {
//                // unselect harvesters
//                for (int i = 0; i < MAX_UNITS; i++) {
//                    cUnit &cUnit = unit[i];
//                    if (!cUnit.isValid()) continue;
//                    if (cUnit.iPlayer != HUMAN) continue;
//                    if (!cUnit.bSelected) continue;
//                    if (cUnit.iType == HARVESTER) {
//                        cUnit.bSelected = false; // unselect
//                    }
//                }
//            }
//
//            if (bPlayInf || bPlayRep) {
//                if (bPlayRep)
//                    play_sound_id(SOUND_REPORTING);
//
//                if (bPlayInf)
//                    play_sound_id(SOUND_YESSIR);
//
//                bOrderingUnits = true;
//            }
//
//        }
//
//        mouse->resetBoxSelect();
//    }
//}

//void cGame::mouseOnBattlefield(int mouseCell, bool &bOrderingUnits) const {
//    const cPlayer &player = players[HUMAN]; // TODO: get player interacting with?
//    const int hover_unit = player.getGameControlsContext()->getIdOfUnitWhereMouseHovers();
//
//    if (mouse->isRightButtonClicked() && !mouse->isMapScrolling()) {
//        UNIT_deselect_all();
//        mouse->setTile(MOUSE_NORMAL);
//    }
//
//    // single clicking and moving
//    if (mouse->isLeftButtonClicked()) {
//        bool bParticle=false;
//
////        if (mouse->isTile(MOUSE_RALLY)) {
////            int id = player.selected_structure;
////            if (id > -1) {
////                if (structure[id]->getOwner() == HUMAN) {
////                    structure[id]->setRallyPoint(mouseCell);
////                    bParticle = true;
////                }
////            }
////        }
//
//        if (hover_unit > -1 && (mouse->isTile(MOUSE_NORMAL) || mouse->isTile(MOUSE_PICK))) {
//            cUnit &hoverUnit = unit[hover_unit];
//            if (hoverUnit.iPlayer == 0) {
//                if (!key[KEY_LSHIFT]) {
//                    UNIT_deselect_all();
//                }
//
//                hoverUnit.bSelected=true;
//
//                if (sUnitInfo[hoverUnit.iType].infantry == false) {
//                    play_sound_id(SOUND_REPORTING);
//                } else {
//                    play_sound_id(SOUND_YESSIR);
//                }
//
//            }
//        } else {
//            bool bPlayInf=false;
//            bool bPlayRep=false;
//
//            if (mouse->isTile(MOUSE_MOVE)) {
//                // any selected unit will move
//                for (int i=0; i < MAX_UNITS; i++) {
//                    cUnit &pUnit = unit[i];
//                    if (!pUnit.isValid()) continue;
//                    if (!pUnit.bSelected) continue;
//                    if (!pUnit.getPlayer()->isHuman()) continue;
//
//                    pUnit.move_to(mouseCell);
//
//                    if (pUnit.isInfantryUnit()) {
//                        bPlayInf = true;
//                    } else {
//                        bPlayRep = true;
//                    }
//
//                    bParticle=true;
//                }
//            } else if (mouse->isTile(MOUSE_ATTACK)) {
//                // check who or what to attack
//                for (int i=0; i < MAX_UNITS; i++) {
//                    cUnit &pUnit = unit[i];
//                    if (!pUnit.isValid()) continue;
//                    if (!pUnit.bSelected) continue;
//                    if (!pUnit.getPlayer()->isHuman()) continue;
//
//                    // Order unit to attack at cell
//                    pUnit.attackAt(mouseCell);
//
//                    if (pUnit.isInfantryUnit()) {
//                        bPlayInf=true;
//                    } else {
//                        bPlayRep=true;
//                    }
//
//                    bParticle=true;
//                }
//            }
//
//            // AUDITIVE FEEDBACK
//            if (bPlayInf || bPlayRep) {
//                if (bPlayInf)
//                    play_sound_id(SOUND_MOVINGOUT + rnd(2));
//
//                if (bPlayRep)
//                    play_sound_id(SOUND_ACKNOWLEDGED + rnd(3));
//
//                bOrderingUnits=true;
//            }
//
//        }
//
//        if (bParticle) {
//            int absoluteXCoordinate = mapCamera->getAbsMapMouseX(mouse_x);
//            int absoluteYCoordinate = mapCamera->getAbsMapMouseY(mouse_y);
//
//            if (mouse->isTile(MOUSE_ATTACK)) {
//                cParticle::create(absoluteXCoordinate, absoluteYCoordinate, D2TM_PARTICLE_ATTACK, -1, -1);
//            } else {
//                cParticle::create(absoluteXCoordinate, absoluteYCoordinate, D2TM_PARTICLE_MOVE, -1, -1);
//            }
//        }
//    }
//}

//void cGame::onCombatMouseEventLeftButtonClicked(const s_MouseEvent &event) {
//    cPlayer &humanPlayer = players[HUMAN];
//    cGameControlsContext *pContext = humanPlayer.getGameControlsContext();
//    const int hoverUnitId = pContext->getIdOfUnitWhereMouseHovers();
//    const int hoverStructureId = pContext->getIdOfStructureWhereMouseHovers();
//    const int mouseCell = pContext->getMouseCell();
//
//    if (mouse->isTile(MOUSE_REPAIR)) {
//        // Mouse is hovering above a unit
//        if (hoverUnitId > -1) {
//            // wanting to repair UNITS, check if this is possible
//            cUnit &hoverUnit = unit[hoverUnitId];
//            if (hoverUnit.iPlayer == HUMAN) {
//                if (hoverUnit.isDamaged() && !hoverUnit.isInfantryUnit() && !hoverUnit.isAirbornUnit()) {
//                    hoverUnit.findBestStructureCandidateAndHeadTowardsItOrWait(REPAIR, true);
//                }
//            }
//        }
//
//        if (hoverStructureId > -1) {
//            cAbstractStructure *pStructure = structure[hoverStructureId];
//
//            if (pStructure) {
//                // toggle between repairing and not repairing
//                pStructure->setRepairing(!pStructure->isRepairing());
//            }
//        }
//    }
//
//    if (mouse->isTile(MOUSE_NORMAL)) {
//        humanPlayer.selected_structure = hoverStructureId;
//
//        // select list that belongs to structure when it is ours
//        cAbstractStructure *theSelectedStructure = structure[humanPlayer.selected_structure];
//        if (theSelectedStructure) {
//            if (theSelectedStructure->getOwner() == HUMAN) {
//                int typeOfStructure = theSelectedStructure->getType();
//                cListUtils listUtils;
//                int listType = listUtils.findListTypeByStructureType(typeOfStructure);
//                if (listType != LIST_NONE) {
//                    humanPlayer.getSideBar()->setSelectedListId(listType);
//                }
//            }
//        } else {
//            humanPlayer.selected_structure = -1;
//        }
//    }
//
//    if (mouse->isTile(MOUSE_RALLY)) {
//        cAbstractStructure *pStructure = humanPlayer.getSelectedStructure();
//        if (pStructure && pStructure->belongsTo(&humanPlayer)) {
//            pStructure->setRallyPoint(mouseCell);
//            int absoluteXCoordinate = mapCamera->getAbsMapMouseX(mouse_x);
//            int absoluteYCoordinate = mapCamera->getAbsMapMouseY(mouse_y);
//
//            cParticle::create(absoluteXCoordinate, absoluteYCoordinate, D2TM_PARTICLE_MOVE, -1, -1);
//        }
//    }
//}
//
//void cGame::onCombatMouseEventRightButtonClicked(const s_MouseEvent &event) {
//    cPlayer &player = players[HUMAN]; // TODO: get player interacting with?
//    cGameControlsContext *context = player.getGameControlsContext();
//
//    if (context->isMouseOnBattleField()) {
//        mouse->dragViewportInteraction();
//    }
//
//    mouse->setTile(MOUSE_NORMAL);
//
//    int structureId = context->getIdOfStructureWhereMouseHovers();
//
//    if (structureId > -1) {
//        cAbstractStructure *pStructure = structure[structureId];
//        if (pStructure->belongsTo(&player)) {
//            if (pStructure->getType() == LIGHTFACTORY ||
//                pStructure->getType() == HEAVYFACTORY ||
//                pStructure->getType() == HIGHTECH ||
//                pStructure->getType() == STARPORT ||
//                pStructure->getType() == WOR ||
//                pStructure->getType() == BARRACKS ||
//                pStructure->getType() == REPAIR)
//                player.setPrimaryBuildingForStructureType(pStructure->getType(), structureId);
//        }
//    }
//}

//int cGame::getMouseTile(const cPlayer *pPlayer) const {
//    const cGameControlsContext *context = pPlayer->getGameControlsContext();
//    int mc = context->getMouseCell();
//    int mouseTile = MOUSE_NORMAL;
//
//    if (mc > -1) { // on battle field
//        int hoverUnitId = context->getIdOfUnitWhereMouseHovers();
//        int hoverStructureId = context->getIdOfStructureWhereMouseHovers();
//
//        // check if any unit is 'selected'
//        const std::vector<int> &selectedUnits = pPlayer->getSelectedUnits();
//
//        if (!selectedUnits.empty()) {
//            }
//        } else {
//            if (map.isVisible(mc, pPlayer->getId())) {
//                if (hoverUnitId > -1) {
//                    cUnit &hoverUnit = unit[hoverUnitId];
//                    if (hoverUnit.iPlayer == HUMAN) {
//                        mouseTile = MOUSE_PICK;
//                    }
//                } else if (hoverStructureId > -1) {
//                    mouse->setTile(MOUSE_NORMAL);
//                }
//            }
//        }
//    }
//
//    // MOVE THIS
//    if (mouseTile == MOUSE_NORMAL) {
//        cAbstractStructure *pSelectedStructure = pPlayer->getSelectedStructure();
//        // when selecting a structure
//        if (pSelectedStructure && pSelectedStructure->belongsTo(pPlayer)) {
//            if (key[KEY_LCONTROL] || key[KEY_RCONTROL]) {
//                mouseTile = MOUSE_RALLY;
//            }
//        }
//    }
//
//    return mouseTile;
//}
