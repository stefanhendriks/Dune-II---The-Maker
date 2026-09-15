/**
 * @file cCreditsState.h
 *
 * Dune 2 - The Maker
 *
 * @author Stefan Hendriks & the D2TM Team
 * @www http://www.dune2themaker.com
 * @copyright Copyright (c) 2002 - 2026 Stefan Hendriks and contributors
 * @license This software is released under the MIT License. See LICENSE.md.
 *
 * Note: Dune 2 is a trademark of Westwood Studios/Electronic Arts.
 *
 * This is a non-commercial educational project.
 */

#pragma once

#include "cGameState.h"
#include "controls/cKeyboardEvent.h"
#include "gui/GuiButton.h"
#include "controls/sMouseEvent.h"
#include "utils/cPoint.h"

#include <string>
#include <vector>
#include <memory>

class Texture;

class cTextDrawer;
class cGameSettings;
class cGameInterface;

struct s_CreditLine {
    std::string name;
    std::string txt;
    Color color;
    int height;
};

class cCreditsState : public cGameState {

public:
    explicit cCreditsState(sGameServices* services);
    ~cCreditsState() override;

    void thinkFast() override;
    void draw() const override;

    void onNotifyMouseEvent(const s_MouseEvent &event) override;
    void onNotifyKeyboardEvent(const cKeyboardEvent &event) override;

    eGameStateType getType() override;

private:
    cTextDrawer* m_textDrawer = nullptr;
    cGameSettings* m_settings = nullptr;
    cGameInterface* m_interface = nullptr;

    int m_titleX;
    float m_crawlerY;

    float m_moveSpeed;
    int m_titleHeight;

    std::vector<s_CreditLine> m_lines;

    cPoint m_duneCoordinates;
    Texture *m_duneBmp;
    Texture *m_titleBmp;
    std::unique_ptr<GuiButton> backButton;

    void resetCrawler();
    void prepareCrawlerLines();
};

