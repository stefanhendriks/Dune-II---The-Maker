#pragma once

#include "cGameState.h"
#include "controls/cKeyboardEvent.h"
#include "drawers/cTextDrawer.h"
#include "sMouseEvent.h"
#include "utils/cRectangle.h"

struct BITMAP;
class cGame;

class cChooseHouseGameState : public cGameState {
  public:
    explicit cChooseHouseGameState(cGame& theGame);
    ~cChooseHouseGameState() override;

    void thinkFast() override;
    void draw() const override;

    void onNotifyMouseEvent(const s_MouseEvent &event) override;
    void onNotifyKeyboardEvent(const cKeyboardEvent &event) override;

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
