/*
 * File:   cKeyboardManager.cpp
 * Author: el.anormal
 *
 * Created Oct 23, 2010
 */

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
            game.bPlaying = false;
        }

        // take screenshot
        if (key[KEY_F11]) {
            takeScreenshot();
        }
    }

    if (key[KEY_F]) {
        alfont_textprintf(bmp_screen, game_font, 0, 44, makecol(255, 255, 255), "FPS/REST: %d / %d", fps, iRest);
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

void cKeyboardManager::takeScreenshot() const {
    char filename[25];

    if (game.screenshot < 10) {
        sprintf(filename, "%dx%d_000%d.bmp", game.screen_x, game.screen_y, game.screenshot);
    } else if (game.screenshot < 100) {
        sprintf(filename, "%dx%d_00%d.bmp", game.screen_x, game.screen_y, game.screenshot);
    } else if (game.screenshot < 1000) {
        sprintf(filename, "%dx%d_0%d.bmp", game.screen_x, game.screen_y, game.screenshot);
    } else {
        sprintf(filename, "%dx%d_%d.bmp", game.screen_x, game.screen_y, game.screenshot);
    }

    save_bmp(filename, bmp_screen, general_palette);

    game.screenshot++;
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
        game.mission_init();
        game.iMission = 9;
        game.iRegion = 22;
        game.setState(GAME_BRIEFING);
        playMusicByType(MUSIC_BRIEFING);
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
                unit[idOfUnitAtCell].setHp(25);
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
        game.mission_init();
        game.iMission = 3;
        game.iRegion = 6;
        game.setState(GAME_BRIEFING);
        playMusicByType(MUSIC_BRIEFING);
        game.createAndPrepareMentatForHumanPlayer();
    }

    //JUMP TO MISSION 4
    if (key[KEY_F7] && players[HUMAN].getHouse() > 0) {
        game.mission_init();
        game.iMission = 4;
        game.iRegion = 10;
        game.setState(GAME_BRIEFING);
        playMusicByType(MUSIC_BRIEFING);
        game.createAndPrepareMentatForHumanPlayer();
    }
    //JUMP TO MISSION 5
    if (key[KEY_F8] && players[HUMAN].getHouse() > 0) {
        game.mission_init();
        game.iMission = 5;
        game.iRegion = 13;
        game.setState(GAME_BRIEFING);
        playMusicByType(MUSIC_BRIEFING);
        game.createAndPrepareMentatForHumanPlayer();
    }

    // take screenshot
    if (key[KEY_F11]) {
        takeScreenshot();
    }
}

// WHen holding CTRL
void cKeyboardManager::GAME_KEYS() {
    int iGroup = game.getGroupNumberFromKeyboard();

    if (key[KEY_Z]) {
        mapCamera->resetZoom();
    }

    // WHEN PRESSED CTRL, MEANING, ADD....
    if (key[KEY_LCONTROL] || key[KEY_RCONTROL]) {
        // UNIT GROUPING
        if (iGroup > 0) {

            // First: Any unit that is already this group number, but NOT selected, must be removed
            for (int i = 0; i < MAX_UNITS; i++) {
                // TODO: This can be done smaller.
                if (unit[i].isValid()) {
                    if (unit[i].iPlayer == 0) {
                        if (unit[i].iGroup == iGroup) {
                            if (unit[i].bSelected == false) {
                                unit[i].iGroup = -1;
                            }
                        }
                    }
                }
            }

            // now add
            for (int i = 0; i < MAX_UNITS; i++) {
                if (unit[i].isValid()) {
                    if (unit[i].iPlayer == 0) {
                        if (unit[i].bSelected) {
                            unit[i].iGroup = iGroup;
                        }
                    }
                }
            }
        }

    }// HOLDING CTRL -> create group
    else {
        // Center on focus cell
        if (key[KEY_H]) {
            mapCamera->centerAndJumpViewPortToCell(players[HUMAN].getFocusCell());
        }

        // Center on the selected structure
        if (key[KEY_C]) {
            if (game.selected_structure > -1) {
                if (structure[game.selected_structure]) {
                    mapCamera->centerAndJumpViewPortToCell(structure[game.selected_structure]->getCell());
                }
            }
        }

        if (iGroup > 0) {
            // First: Any unit that is already this group number, but NOT selected, must be removed

            // not pressing shift, meaning, we remove all selected stuff
            if (key[KEY_LSHIFT] == false) {
                for (int i = 0; i < MAX_UNITS; i++) {
                    if (unit[i].isValid()) {
                        if (unit[i].iPlayer == 0) {
                            if (unit[i].bSelected) {
                                unit[i].bSelected = false;
                            }
                        }
                    }
                }
            }

            bool bPlayRep = false;
            bool bPlayInf = false;
            // now add
            for (int i = 0; i < MAX_UNITS; i++) {
                if (unit[i].isValid())
                    if (unit[i].iPlayer == 0)
                        if (unit[i].iGroup == iGroup) {
                            unit[i].bSelected = true;

                            if (sUnitInfo[unit[i].iType].infantry) {
                                bPlayInf = true;
                            } else {
                                bPlayRep = true;
                            }
                        }
            }

            // HACK HACK: This should actually not be randomized. This is done
            // so you will not hear 100x "yes sir" at a time, blowing your speakers
            if (rnd(100) < 15) {
                if (bPlayRep) {
                    play_sound_id(SOUND_REPORTING);
                }

                if (bPlayInf) {
                    play_sound_id(SOUND_YESSIR);
                }
            } // END HACK
        }
    }
}

