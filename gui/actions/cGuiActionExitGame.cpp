#include "d2tmh.h"

void cGuiActionExitGame::execute() {
    game.m_playing = false;
    game.initiateFadingOut();
}