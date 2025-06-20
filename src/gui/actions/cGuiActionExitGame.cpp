#include "cGuiActionExitGame.h"

#include "d2tmc.h"

void cGuiActionExitGame::execute()
{
    game.m_playing = false;
    game.initiateFadingOut();
}