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
    int m_titleX;
    float m_crawlerY;

    float m_moveSpeed;
    int m_titleHeight;

    const cTextDrawer m_textDrawer;

    std::vector<s_CreditLine> m_lines;

    cPoint m_duneCoordinates;
    BITMAP *m_duneBmp;
    BITMAP *m_titleBmp;

    void resetCrawler();
    void prepareCrawlerLines();
};

