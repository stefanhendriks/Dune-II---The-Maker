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
    void draw() const override;

    void onNotifyMouseEvent(const s_MouseEvent &event) override;
    void onNotifyKeyboardEvent(const s_KeyboardEvent &event) override;

    eGameStateType getType() override;

private:
    cTextDrawer textDrawer;
    cRectangle *backButtonRect;

    BITMAP *bmp_Dune;
    BITMAP *bmp_SelectYourHouseTitle;

    // TODO: abstract away the selectable houses
    BITMAP *bmp_HouseAtreides;
    BITMAP *bmp_HouseOrdos;
    BITMAP *bmp_HouseHarkonnen;

    // Click areas of house emblems
    cRectangle houseAtreides;
    cRectangle houseOrdos;
    cRectangle houseHarkonnen;

    int selectYourHouseXCentered;
    cPoint coords_Dune;
    cPoint coords_SelectYourHouseTitle;

    // Functions
    bool hoversOverBackButton;

    void onMouseLeftButtonClicked(const s_MouseEvent &event) const;

    void onMouseMoved(const s_MouseEvent &event);
};

#endif //D2TM_CCHOOSEHOUSEGAMESTATE_H
