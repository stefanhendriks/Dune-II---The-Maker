/*

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefan@fundynamic.com
  Website: http://dune2themaker.fundynamic.com

  2001 - 2021 (c) code by Stefan Hendriks
*/

#include "include/d2tmh.h"

// Fading between menu items
void cGame::START_FADING_OUT() {
    // set state to fade out
    fadeAction = eFadeAction::FADE_OUT; // fade out

    // copy the last bitmap of screen into a separate bitmap which we use for fading out.
    draw_sprite(bmp_fadeout, bmp_screen, 0, 0);
}

// this shows the you have lost bmp at screen, after mouse press the mentat debriefing state will begin
void cGame::losing() {
    blit(bmp_winlose, bmp_screen, 0, 0, 0, 0, screen_x, screen_y);

    draw_sprite(bmp_screen, (BITMAP *) gfxdata[MOUSE_NORMAL].dat, mouse_x, mouse_y);

    if (mouse->isLeftButtonClicked()) {
        // OMG, MENTAT IS NOT HAPPY
        state = GAME_LOSEBRIEF;

        if (bSkirmish) {
            game.mission_init();
        }

        createAndPrepareMentatForHumanPlayer();

        // FADE OUT
        START_FADING_OUT();
    }
}

// this shows the you have won bmp at screen, after mouse press the mentat debriefing state will begin
void cGame::winning() {
    blit(bmp_winlose, bmp_screen, 0, 0, 0, 0, screen_x, screen_y);

    draw_sprite(bmp_screen, (BITMAP *) gfxdata[MOUSE_NORMAL].dat, mouse_x, mouse_y);

    if (mouse->isLeftButtonClicked()) {
        // Mentat will be happy, after that enter "Select your next Conquest"
        state = GAME_WINBRIEF;

        if (bSkirmish) {
            game.mission_init();
        }

        createAndPrepareMentatForHumanPlayer();

        // FADE OUT
        START_FADING_OUT();
    }
}

void cGame::onCombatMouseEvent(const s_MouseEvent &event) {
    cPlayer &player = players[HUMAN]; // TODO: get player interacting with?
    cGameControlsContext *context = player.getGameControlsContext();
    bool bOrderingUnits=false;

    if (player.isNotPlacingSomething() && player.isNotDeployingSomething()) {
        combat_mouse_normalCombatInteraction(player, bOrderingUnits, context->getMouseCell());
    }

    switch (event.eventType) {
        case MOUSE_RIGHT_BUTTON_PRESSED:
            onCombatMouseEventRightButtonClicked(event);
            break;
        case MOUSE_MOVED_TO:
            onCombatMouseEventMovedTo(event);
            break;
        case MOUSE_LEFT_BUTTON_CLICKED:
            if (context->isMouseOnBattleField()) {
                onCombatMouseEventLeftButtonClicked(event);
            }
            break;
        default:
            // set to -1 only when it was > -1
            mouse->resetDragViewportInteraction();
            break;
    }

	if (bOrderingUnits) {
        player.deselectStructure();
	}

}

void cGame::mouse_combat_hoverOverStructureInteraction(cPlayer &player, cGameControlsContext *context,
                                                       bool bOrderingUnits) const {
    int structureIdWhereMouseHovers = context->getIdOfStructureWhereMouseHovers();

    // REPAIR
    if (key[KEY_R] && !bOrderingUnits) {
        int structureId = structureIdWhereMouseHovers;

        if (structure[structureId]->getOwner() == HUMAN &&
            structure[structureId]->isDamaged()) {
            mouse->setTile(MOUSE_REPAIR);
        }
    } // MOUSE PRESSED
}

void
cGame::combat_mouse_normalCombatInteraction(cPlayer &humanPlayer, bool &bOrderingUnits, int mouseCell) const {
    // when mouse hovers above a valid cell
    if (mouseCell > -1) {
        mouseOnBattlefield(mouseCell, bOrderingUnits);
    }

    if (mouse->isLeftButtonPressed()) {
        mouse->boxSelectLogic(mouseCell);
    } else {
        // where we box selecting? then this must be the unpress of the mouse button and thus we
        // should start selecting units within the rectangle
        if (mouse->isBoxSelecting()) {
            cRectangle boxSelectRectangle = mouse->getBoxSelectRectangle();

            //  char msg[256];
            //  sprintf(msg, "MINX=%d, MAXX=%d, MINY=%d, MAXY=%d", min_x, min_y, max_x, max_y);
            //  logbook(msg);

            // Now do it!
            // deselect all units
            UNIT_deselect_all();
            mouse->setTile(MOUSE_NORMAL);

            bool bPlayRep = false;
            bool bPlayInf = false;

            bool harvesterSelected=false;
            bool attackingUnitSelected=false;

            for (int i = 0; i < MAX_UNITS; i++) {
                cUnit &cUnit = unit[i];
                if (!cUnit.isValid()) continue;
                if (cUnit.iPlayer != HUMAN) continue;
                // do not select airborn units
                if (cUnit.isAirbornUnit()) {
                    // always deselect unit:
                    cUnit.bSelected = false;
                    continue;
                }
                if (cUnit.iTempHitPoints >= 0) continue; // skip units that have been 'hidden' in a refinery or repair

                if (boxSelectRectangle.isPointWithin(cUnit.center_draw_x(), cUnit.center_draw_y())) {
                    // It is in the borders, select it
                    cUnit.bSelected = true;

                    if (cUnit.iType == HARVESTER) {
                        harvesterSelected = true;
                    } else {
                        attackingUnitSelected = true;
                    }

                    if (sUnitInfo[cUnit.iType].infantry) {
                        bPlayInf = true;
                    } else {
                        bPlayRep = true;
                    }

                }
            }

            if (harvesterSelected && attackingUnitSelected) {
                // unselect harvesters
                for (int i = 0; i < MAX_UNITS; i++) {
                    cUnit &cUnit = unit[i];
                    if (!cUnit.isValid()) continue;
                    if (cUnit.iPlayer != HUMAN) continue;
                    if (!cUnit.bSelected) continue;
                    if (cUnit.iType == HARVESTER) {
                        cUnit.bSelected = false; // unselect
                    }
                }
            }

            if (bPlayInf || bPlayRep) {
                if (bPlayRep)
                    play_sound_id(SOUND_REPORTING);

                if (bPlayInf)
                    play_sound_id(SOUND_YESSIR);

                bOrderingUnits = true;
            }

        }

        mouse->resetBoxSelect();
    }
}

void cGame::mouseOnBattlefield(int mouseCell, bool &bOrderingUnits) const {
    const cPlayer &player = players[HUMAN]; // TODO: get player interacting with?
    const int hover_unit = player.getGameControlsContext()->getIdOfUnitWhereMouseHovers();

    if (mouse->isRightButtonClicked() && !mouse->isMapScrolling()) {
        UNIT_deselect_all();
        mouse->setTile(MOUSE_NORMAL);
    }

    // single clicking and moving
    if (mouse->isLeftButtonClicked()) {
        bool bParticle=false;

//        if (mouse->isTile(MOUSE_RALLY)) {
//            int id = player.selected_structure;
//            if (id > -1) {
//                if (structure[id]->getOwner() == HUMAN) {
//                    structure[id]->setRallyPoint(mouseCell);
//                    bParticle = true;
//                }
//            }
//        }

        if (hover_unit > -1 && (mouse->isTile(MOUSE_NORMAL) || mouse->isTile(MOUSE_PICK))) {
            cUnit &hoverUnit = unit[hover_unit];
            if (hoverUnit.iPlayer == 0) {
                if (!key[KEY_LSHIFT]) {
                    UNIT_deselect_all();
                }

                hoverUnit.bSelected=true;

                if (sUnitInfo[hoverUnit.iType].infantry == false) {
                    play_sound_id(SOUND_REPORTING);
                } else {
                    play_sound_id(SOUND_YESSIR);
                }

            }
        } else {
            bool bPlayInf=false;
            bool bPlayRep=false;

            if (mouse->isTile(MOUSE_MOVE)) {
                // any selected unit will move
                for (int i=0; i < MAX_UNITS; i++) {
                    cUnit &pUnit = unit[i];
                    if (!pUnit.isValid()) continue;
                    if (!pUnit.bSelected) continue;
                    if (!pUnit.getPlayer()->isHuman()) continue;

                    pUnit.move_to(mouseCell);

                    if (pUnit.isInfantryUnit()) {
                        bPlayInf = true;
                    } else {
                        bPlayRep = true;
                    }

                    bParticle=true;
                }
            } else if (mouse->isTile(MOUSE_ATTACK)) {
                // check who or what to attack
                for (int i=0; i < MAX_UNITS; i++) {
                    cUnit &pUnit = unit[i];
                    if (!pUnit.isValid()) continue;
                    if (!pUnit.bSelected) continue;
                    if (!pUnit.getPlayer()->isHuman()) continue;

                    // Order unit to attack at cell
                    pUnit.attackAt(mouseCell);

                    if (pUnit.isInfantryUnit()) {
                        bPlayInf=true;
                    } else {
                        bPlayRep=true;
                    }

                    bParticle=true;
                }
            }

            // AUDITIVE FEEDBACK
            if (bPlayInf || bPlayRep) {
                if (bPlayInf)
                    play_sound_id(SOUND_MOVINGOUT + rnd(2));

                if (bPlayRep)
                    play_sound_id(SOUND_ACKNOWLEDGED + rnd(3));

                bOrderingUnits=true;
            }

        }

        if (bParticle) {
            int absoluteXCoordinate = mapCamera->getAbsMapMouseX(mouse_x);
            int absoluteYCoordinate = mapCamera->getAbsMapMouseY(mouse_y);

            if (mouse->isTile(MOUSE_ATTACK)) {
                cParticle::create(absoluteXCoordinate, absoluteYCoordinate, D2TM_PARTICLE_ATTACK, -1, -1);
            } else {
                cParticle::create(absoluteXCoordinate, absoluteYCoordinate, D2TM_PARTICLE_MOVE, -1, -1);
            }
        }
    }
}
