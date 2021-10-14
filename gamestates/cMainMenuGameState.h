#ifndef D2TM_CMAINMENUGAMESTATE_H
#define D2TM_CMAINMENUGAMESTATE_H

class cGame;

#include "cGameState.h"

class cMainMenuGameState : public cGameState {

public:
    cMainMenuGameState(cGame& theGame);
    ~cMainMenuGameState() override;

    void thinkFast() override;
    void draw() override;

    void onNotifyMouseEvent(const s_MouseEvent &event) override;

    eGameStateType getType() override;

private:
    int logoX;
    int logoY;

    int mainMenuFrameX;
    int mainMenuFrameY;
    int mainMenuWidth;
    int mainMenuHeight;

    cTextDrawer textDrawer;

    BITMAP *bmp_D2TM_Title;
};


#endif //D2TM_CMAINMENUGAMESTATE_H
