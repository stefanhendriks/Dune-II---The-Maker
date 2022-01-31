#ifndef D2TM_CMAINMENUGAMESTATE_H
#define D2TM_CMAINMENUGAMESTATE_H

class cGame;

#include <gui/cGuiButton.h>
#include <gui/cGuiWindow.h>
#include "cGameState.h"

class cMainMenuGameState : public cGameState {

public:
    cMainMenuGameState(cGame& theGame);
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


#endif //D2TM_CMAINMENUGAMESTATE_H
