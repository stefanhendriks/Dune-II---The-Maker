#pragma once

#include "cGameState.h"
#include "controls/cKeyboardEvent.h"
#include "drawers/cTextDrawer.h"
#include "gui/cGuiButton.h"
#include "gui/cGuiWindow.h"
#include "sMouseEvent.h"

struct BITMAP;
class cGame;

class cMainMenuGameState : public cGameState {
  public:
    explicit cMainMenuGameState(cGame& theGame);
    ~cMainMenuGameState() override;

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

    const cTextDrawer textDrawer;

    cGuiWindow * gui_window;
    cGuiButton * gui_btn_credits;

    BITMAP *bmp_D2TM_Title;
};
