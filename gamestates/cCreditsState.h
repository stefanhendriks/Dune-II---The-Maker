#pragma once

class cGame;

#include <gui/cGuiButton.h>
#include <gui/cGuiWindow.h>
#include "cGameState.h"

struct s_CreditLine {
    std::string txt;
    int color;
    int height;
};

class cCreditsState : public cGameState {

public:
    explicit cCreditsState(cGame& theGame);
    ~cCreditsState() override;

    void thinkFast() override;
    void draw() const override;

    void onNotifyMouseEvent(const s_MouseEvent &event) override;
    void onNotifyKeyboardEvent(const cKeyboardEvent &event) override;

    eGameStateType getType() override;

private:
    int logoX;
    float crawlerY;
    float moveSpeed;
    int logoHeight;

    const cTextDrawer textDrawer;

    std::vector<s_CreditLine> lines;

    cPoint coords_Dune;
    BITMAP *bmp_Dune;
    BITMAP *bmp_D2TM_Title;

    void resetCrawler();

    void prepareCrawlerLines();
};

