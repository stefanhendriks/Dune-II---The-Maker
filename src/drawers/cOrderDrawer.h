/**
 * @file cOrderDrawer.h
 *
 * Dune 2 - The Maker
 *
 * @author Stefan Hendriks & the D2TM Team
 * @www http://www.dune2themaker.com
 * @copyright Copyright (c) 2002 - 2026 Stefan Hendriks and contributors
 * @license This software is released under the MIT License. See LICENSE.md.
 *
 * Note: Dune 2 is a trademark of Westwood Studios/Electronic Arts.
 *
 * This is a non-commercial educational project.
 */

#pragma once

#include "controls/sMouseEvent.h"
#include "utils/cRectangle.h"

class cPlayer;
struct SDL_Surface;
class Texture;
class GameContext;
class SDLDrawer;
struct sGameServices;

class cOrderDrawer {
public:
    explicit cOrderDrawer(GameContext *ctx, cPlayer *player);

    ~cOrderDrawer();

    void drawOrderButton(cPlayer *thePlayer);

    void onNotify(const s_MouseEvent &event);

    void serviceInit(sGameServices*) {}

    void setPlayer(cPlayer *pPlayer);

private:
    void drawRectangleOrderButton();

    void onMouseAt(const s_MouseEvent &event);
    void onMouseClickedLeft(const s_MouseEvent &event);

    bool m_isMouseOverOrderButton;
    GameContext *m_ctx;
    SDLDrawer *m_sdlDrawer;
    cPlayer *m_player;
    cRectangle m_buttonRect;
    Texture *m_buttonBitmap;
};
