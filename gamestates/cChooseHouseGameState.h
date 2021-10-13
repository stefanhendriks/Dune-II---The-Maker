#ifndef D2TM_CCHOOSEHOUSEGAMESTATE_H
#define D2TM_CCHOOSEHOUSEGAMESTATE_H

class cGame;

#include <drawers/cTextDrawer.h>
#include "cGameState.h"

class cChooseHouseGameState : public cGameState {

public:
    cChooseHouseGameState(cGame& theGame);
    ~cChooseHouseGameState() override;

    void thinkFast() override;
    void draw() override;

    void onNotifyMouseEvent(const s_MouseEvent &event) override;

    eGameStateType getType() override;

private:
    void onMouseLeftButtonClicked(const s_MouseEvent &event);
};

#endif //D2TM_CCHOOSEHOUSEGAMESTATE_H
