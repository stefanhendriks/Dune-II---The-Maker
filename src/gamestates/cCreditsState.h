#pragma once

#include "cGameState.h"
#include "controls/cKeyboardEvent.h"
#include "drawers/cTextDrawer.h"
#include "sMouseEvent.h"
#include "utils/cPoint.h"

#include <string>
#include <vector>

class Texture;
class cGame;

struct s_CreditLine {
    std::string name;
    std::string txt;
    Color color;
    int height;
};

class cCreditsState : public cGameState {

public:
    explicit cCreditsState(cGame &theGame, GameContext* ctx);
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

    std::shared_ptr<cTextDrawer> m_textDrawer = nullptr;

    std::vector<s_CreditLine> m_lines;

    cPoint m_duneCoordinates;
    Texture *m_duneBmp;
    Texture *m_titleBmp;

    void resetCrawler();
    void prepareCrawlerLines();
};

