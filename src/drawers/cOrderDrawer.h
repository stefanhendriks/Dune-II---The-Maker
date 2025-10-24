#pragma once

#include "sMouseEvent.h"
#include "utils/cRectangle.h"

class cPlayer;
struct SDL_Surface;
class Texture;
class GameContext;

class cOrderDrawer {
public:
    explicit cOrderDrawer(GameContext *ctx, cPlayer *thePlayer);

    ~cOrderDrawer();

    void drawOrderButton(cPlayer *thePlayer);

    void onNotify(const s_MouseEvent &event);

    void setPlayer(cPlayer *pPlayer);

private:
    void drawRectangleOrderButton();

    void onMouseAt(const s_MouseEvent &event);
    void onMouseClickedLeft(const s_MouseEvent &event);

    bool _isMouseOverOrderButton;
    GameContext *m_ctx;
    cPlayer *player;
    cRectangle buttonRect;
    Texture *buttonBitmap;
};
