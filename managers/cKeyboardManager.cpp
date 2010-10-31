/*
 * File:   cKeyboardManager.cpp
 * Author: el.anormal
 *
 * Created Oct 23, 2010
 */

#include "../d2tmh.h"

cKeyboardManager::cKeyboardManager() {
}

cKeyboardManager::cKeyboardManager(const cKeyboardManager& orig) {
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


    }

    if (key[KEY_F]) {
        alfont_textprintf(bmp_screen, game_font, 0, 44, makecol(255, 255, 255), "FPS: %d", fps);
    }

    /* Handle here keys that are only active when debugging */
    if (DEBUGGING) {

        if (key[KEY_F4]) {
            if (player[HUMAN].getGameControlsContext()->getMouseCell() > -1) {
                map.clear_spot(player[HUMAN].getGameControlsContext()->getMouseCell(), 3, 0);
            }
        }
    }
}

void cKeyboardManager::DEBUG_KEYS() {
    //JUMP TO MISSION 9
    if (key[KEY_F1] && game.iHouse > 0) {
        game.mission_init();
        game.iMission = 9;
        game.iRegion = 22;
        game.iWinQuota = -1;
        game.setState(GAME_BRIEFING);
        play_music(MUSIC_BRIEFING);
        game.iMentatSpeak = -1;
    }
    // WIN MISSION
    if (key[KEY_F2]) {
        if (game.iWinQuota > -1)
            player[0].credits = game.iWinQuota + 1;
        else {
            game.destroyAllStructures(false);
            game.destroyAllUnits(false);
        }
    }
    // LOSE MISSION
    if (key[KEY_F3]) {
        game.destroyAllStructures(true);
        game.destroyAllUnits(true);
    }
    // GIVE 299999 CREDITS TO PLAYER
    if (key[KEY_F4] && !key[KEY_LSHIFT]) {
        player[0].credits = 299999;
    }
    //DESTROY UNIT OR BUILDING
    if (key[KEY_F4] && key[KEY_LSHIFT]) {
        int mc = player[HUMAN].getGameControlsContext()->getMouseCell();
        if (mc > -1) {
            if (map.cell[mc].id[MAPID_UNITS] > -1) {
                int id = map.cell[mc].id[MAPID_UNITS];
                unit[id].die(true, false);
            }

            if (map.cell[mc].id[MAPID_STRUCTURES] > -1) {
                int id = map.cell[mc].id[MAPID_STRUCTURES];
                structure[id]->die();
            }
        }
    }
    // REVEAL  MAP
    if (key[KEY_F5]) {
        map.clear_all();
    }
    //JUMP TO MISSION 3
    if (key[KEY_F6] && game.iHouse > 0) {
        game.mission_init();
        game.iMission = 3;
        game.iRegion = 6;
        game.iWinQuota = -1;
        game.setState(GAME_BRIEFING);
        play_music(MUSIC_BRIEFING);
        game.iMentatSpeak = -1;
    }
    //JUMP TO MISSION 4
    if (key[KEY_F7] && game.iHouse > 0) {
        game.mission_init();
        game.iMission = 4;
        game.iRegion = 10;
        game.iWinQuota = -1;
        game.setState(GAME_BRIEFING);
        play_music(MUSIC_BRIEFING);
        game.iMentatSpeak = -1;
    }
    //JUMP TO MISSION 5
    if (key[KEY_F8] && game.iHouse > 0) {
        game.mission_init();
        game.iMission = 5;
        game.iRegion = 13;
        game.iWinQuota = -1;
        game.setState(GAME_BRIEFING);
        play_music(MUSIC_BRIEFING);
        game.iMentatSpeak = -1;
    }
}

// WHen holding CTRL
void cKeyboardManager::GAME_KEYS() {
    int iGroup = game.getGroupNumberFromKeyboard();

    // WHEN PRESSED CTRL, MEANING, ADD....
    if (key[KEY_LCONTROL]) {
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
            mapCamera->centerAndJumpViewPortToCell(player[HUMAN].focus_cell);
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

                            if (units[unit[i].iType].infantry) {
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
                    play_sound_id(SOUND_REPORTING, -1);
                }

                if (bPlayInf) {
                    play_sound_id(SOUND_YESSIR, -1);
                }
            } // END HACK
        }
    }
}

