#pragma once

#include "cGameState.h"
#include "controls/cKeyboardEvent.h"
#include "drawers/cTextDrawer.h"
#include "gui/GuiButton.h"
#include "gui/GuiWindow.h"
#include "sMouseEvent.h"

class Texture;
class cGame;

class cMainMenuState : public cGameState {
public:
    explicit cMainMenuState(cGame &theGame, GameContext* ctx);
    ~cMainMenuState() override;

    void thinkFast() override;
    void draw() const override;

    void onNotifyMouseEvent(const s_MouseEvent &event) override;
    void onNotifyKeyboardEvent(const cKeyboardEvent &event) override;

    eGameStateType getType() override;

private:
    int logoX;
    int logoY;

    int mainMenuFrameX;
    int mainMenuFrameY;
    int mainMenuWidth;
    int mainMenuHeight;

    cTextDrawer textDrawer;
    cRectangle sdl2power;

    GuiWindow *gui_window;
    GuiButton *gui_btn_credits;

    Texture *bmp_D2TM_Title;
    Texture *backGroundDebug;
};
