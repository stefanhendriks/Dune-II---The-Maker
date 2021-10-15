#ifndef D2TM_CMAINMENUGAMESTATE_H
#define D2TM_CMAINMENUGAMESTATE_H

class cGame;

#include <gui/cGuiButton.h>
#include "cGameState.h"

class cMainMenuGameState : public cGameState {

public:
    cMainMenuGameState(cGame& theGame);
    ~cMainMenuGameState() override;

    void thinkFast() override;
    void draw() const override;

    void onNotifyMouseEvent(const s_MouseEvent &event) override;

    eGameStateType getType() override;

private:
    int logoX;
    int logoY;

    int mainMenuFrameX;
    int mainMenuFrameY;
    int mainMenuWidth;
    int mainMenuHeight;

    const cTextDrawer textDrawer;

    cGuiButton * gui_btn_SelectHouse;
    cGuiButton * gui_btn_Skirmish;
    cGuiButton * gui_btn_Multiplayer;
    cGuiButton * gui_btn_Load;
    cGuiButton * gui_btn_Options;
    cGuiButton * gui_btn_Hof;
    cGuiButton * gui_btn_Exit;

    BITMAP *bmp_D2TM_Title;

    void onMouseLeftButtonClicked(const s_MouseEvent &event);
};


#endif //D2TM_CMAINMENUGAMESTATE_H
