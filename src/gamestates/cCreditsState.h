#pragma once

#include "cGameState.h"
#include "controls/cKeyboardEvent.h"
#include "gui/GuiButton.h"
#include "sMouseEvent.h"
#include "utils/cPoint.h"

#include <string>
#include <vector>
#include <memory>

class Texture;
class cGame;
class cTextDrawer;

struct s_CreditLine {
    std::string name;
    std::string txt;
    Color color;
    int height;
};

class cCreditsState : public cGameState {

public:
    explicit cCreditsState(cGame &theGame, sGameServices* services);
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

    cTextDrawer* m_textDrawer = nullptr;

    std::vector<s_CreditLine> m_lines;

    cPoint m_duneCoordinates;
    Texture *m_duneBmp;
    Texture *m_titleBmp;
    std::unique_ptr<GuiButton> backButton;

    void resetCrawler();
    void prepareCrawlerLines();
};

