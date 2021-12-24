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

