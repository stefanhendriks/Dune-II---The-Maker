#pragma once

#include "sMouseEvent.h"
#include "utils/cRectangle.h"

class cPlayer;
struct SDL_Surface;
class Texture;
class GameContext;

class cOrderDrawer {
public:
    explicit cOrderDrawer(GameContext *ctx, cPlayer *player);

    ~cOrderDrawer();

    void drawOrderButton(cPlayer *thePlayer);

    void onNotify(const s_MouseEvent &event);

    void setPlayer(cPlayer *pPlayer);

private:
    void drawRectangleOrderButton();

    void onMouseAt(const s_MouseEvent &event);
    void onMouseClickedLeft(const s_MouseEvent &event);

    bool m_isMouseOverOrderButton;
    GameContext *m_ctx;
    cPlayer *m_player;
    cRectangle m_buttonRect;
    Texture *m_buttonBitmap;
};
