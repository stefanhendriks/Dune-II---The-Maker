#include "../include/d2tmh.h"

cKeyboardManager::cKeyboardManager() {
}

cKeyboardManager::~cKeyboardManager() {
}

/**
        Handle keyboard keys.
        TAB + key = debug action
 */
void cKeyboardManager::interact() {
    if (key[KEY_TAB]) {
        DEBUG_KEYS();
    } else {
        GAME_KEYS();

        if (key[KEY_ESC]) {
            game.setNextStateToTransitionTo(GAME_OPTIONS);
        }
    }

    if (key[KEY_F]) {
        alfont_textprintf(bmp_screen, game_font, 0, 44, makecol(255, 255, 255), "FPS/REST: %d / %d", game.getFps(), iRest);
    }

    /* Handle here keys that are only active when debugging */
    if (DEBUGGING) {

        if (key[KEY_F4]) {
            if (players[HUMAN].getGameControlsContext()->getMouseCell() > -1) {
                map.clearShroud(players[HUMAN].getGameControlsContext()->getMouseCell(), 6, HUMAN);
            }
        }
    }
}

void cKeyboardManager::DEBUG_KEYS() {
    if (key[KEY_0]) {
        drawManager->setPlayerToDraw(&players[0]);
        game.setPlayerToInteractFor(&players[0]);
    } else if (key[KEY_1]) {
        drawManager->setPlayerToDraw(&players[1]);
        game.setPlayerToInteractFor(&players[1]);
    } else if (key[KEY_2]) {
        drawManager->setPlayerToDraw(&players[2]);
        game.setPlayerToInteractFor(&players[2]);
    } else if (key[KEY_3]) {
        drawManager->setPlayerToDraw(&players[3]);
        game.setPlayerToInteractFor(&players[3]);
    }

    //JUMP TO MISSION 9
    if (key[KEY_F1] && players[HUMAN].getHouse() > 0) {
        game.missionInit();
        game.m_mission = 9;
        game.m_region = 22;
        game.setNextStateToTransitionTo(GAME_BRIEFING);
        game.playMusicByType(MUSIC_BRIEFING);
        game.createAndPrepareMentatForHumanPlayer();
    }

    // WIN MISSION
    if (key[KEY_F2]) {
        game.setMissionWon();
    }

    // LOSE MISSION
    if (key[KEY_F3]) {
        game.setMissionLost();
    }

    // GIVE CREDITS TO ALL PLAYERS
    if (key[KEY_F4] && !key[KEY_LSHIFT]) {
        for (int i = 0; i < AI_WORM; i++) {
            players[i].setCredits(5000);
        }
    }

    //DESTROY UNIT OR BUILDING
    if (key[KEY_F4] && key[KEY_LSHIFT]) {
        int mc = players[HUMAN].getGameControlsContext()->getMouseCell();
        if (mc > -1) {
            int idOfUnitAtCell = map.getCellIdUnitLayer(mc);
            if (idOfUnitAtCell > -1) {
                unit[idOfUnitAtCell].die(true, false);
            }

            int idOfStructureAtCell = map.getCellIdStructuresLayer(mc);
            if (idOfStructureAtCell > -1) {
                structure[idOfStructureAtCell]->die();
            }

            idOfUnitAtCell = map.getCellIdWormsLayer(mc);
            if (idOfUnitAtCell > -1) {
                unit[idOfUnitAtCell].die(false, false);
            }
        }
    }

    //DESTROY UNIT OR BUILDING
    if (key[KEY_F5] && key[KEY_LSHIFT]) {
        int mc = players[HUMAN].getGameControlsContext()->getMouseCell();
        if (mc > -1) {
            int idOfUnitAtCell = map.getCellIdUnitLayer(mc);
            if (idOfUnitAtCell > -1) {
                cUnit &pUnit = unit[idOfUnitAtCell];
                int damageToTake = pUnit.getHitPoints() - 25;
                if (damageToTake > 0) {
                    pUnit.takeDamage(damageToTake);
                }
            }
        }
    } else {
        // REVEAL  MAP
        if (key[KEY_F5]) {
            map.clear_all(HUMAN);
        }
    }

    //JUMP TO MISSION 3
    if (key[KEY_F6] && players[HUMAN].getHouse() > 0) {
        game.missionInit();
        game.m_mission = 3;
        game.m_region = 6;
        game.setNextStateToTransitionTo(GAME_BRIEFING);
        game.playMusicByType(MUSIC_BRIEFING);
        game.createAndPrepareMentatForHumanPlayer();
    }

    //JUMP TO MISSION 4
    if (key[KEY_F7] && players[HUMAN].getHouse() > 0) {
        game.missionInit();
        game.m_mission = 4;
        game.m_region = 10;
        game.setNextStateToTransitionTo(GAME_BRIEFING);
        game.playMusicByType(MUSIC_BRIEFING);
        game.createAndPrepareMentatForHumanPlayer();
    }
    //JUMP TO MISSION 5
    if (key[KEY_F8] && players[HUMAN].getHouse() > 0) {
        game.missionInit();
        game.m_mission = 5;
        game.m_region = 13;
        game.setNextStateToTransitionTo(GAME_BRIEFING);
        game.playMusicByType(MUSIC_BRIEFING);
        game.createAndPrepareMentatForHumanPlayer();
    }
}

// WHen holding CTRL
void cKeyboardManager::GAME_KEYS() {
//    int iGroup = game.getGroupNumberFromKeyboard();

    if (key[KEY_Z]) {
        mapCamera->resetZoom();
    }

    // WHEN PRESSED CTRL, MEANING, ADD....
    if (key[KEY_LCONTROL] || key[KEY_RCONTROL]) {
//        // UNIT GROUPING
//        if (iGroup > 0) {
//
//            // First: Any unit that is already this group number, but NOT selected, must be removed
//            for (int i = 0; i < MAX_UNITS; i++) {
//                // TODO: This can be done smaller.
//                if (unit[i].isValid()) {
//                    if (unit[i].iPlayer == 0) {
//                        if (unit[i].iGroup == iGroup) {
//                            if (unit[i].bSelected == false) {
//                                unit[i].iGroup = -1;
//                            }
//                        }
//                    }
//                }
//            }
//
//            // now add
//            for (int i = 0; i < MAX_UNITS; i++) {
//                if (unit[i].isValid()) {
//                    if (unit[i].iPlayer == 0) {
//                        if (unit[i].bSelected) {
//                            unit[i].iGroup = iGroup;
//                        }
//                    }
//                }
//            }
//        }

    }// HOLDING CTRL -> create group
    else {
        // Center on focus cell
        cPlayer &humanPlayer = players[HUMAN];

        if (key[KEY_H]) {
            mapCamera->centerAndJumpViewPortToCell(humanPlayer.getFocusCell());
        }

        // Center on the selected structure
        if (key[KEY_C]) {
            cAbstractStructure *selectedStructure = humanPlayer.getSelectedStructure();
            if (selectedStructure) {
                mapCamera->centerAndJumpViewPortToCell(selectedStructure->getCell());
            }
        }

//        if (iGroup > 0) {
//            // First: Any unit that is already this group number, but NOT selected, must be removed
//
//            // not pressing shift, meaning, we remove all selected stuff
//            if (key[KEY_LSHIFT] == false) {
//                for (int i = 0; i < MAX_UNITS; i++) {
//                    if (unit[i].isValid()) {
//                        if (unit[i].iPlayer == 0) {
//                            if (unit[i].bSelected) {
//                                unit[i].bSelected = false;
//                            }
//                        }
//                    }
//                }
//            }
//
//            bool bPlayRep = false;
//            bool bPlayInf = false;
//            // now add
//            for (int i = 0; i < MAX_UNITS; i++) {
//                if (unit[i].isValid())
//                    if (unit[i].iPlayer == 0)
//                        if (unit[i].iGroup == iGroup) {
//                            unit[i].bSelected = true;
//
//                            if (sUnitInfo[unit[i].iType].infantry) {
//                                bPlayInf = true;
//                            } else {
//                                bPlayRep = true;
//                            }
//                        }
//            }
//
//            // HACK HACK: This should actually not be randomized. This is done
//            // so you will not hear 100x "yes sir" at a time, blowing your speakers
//            if (rnd(100) < 15) {
//                if (bPlayRep) {
//                    play_sound_id(SOUND_REPORTING);
//                }
//
//                if (bPlayInf) {
//                    play_sound_id(SOUND_YESSIR);
//                }
//            } // END HACK
//        }
    }
}

