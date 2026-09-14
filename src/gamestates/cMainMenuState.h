/**
 * @file cMainMenuState.h
 *
 * Dune 2 - The Maker
 *
 * @author Stefan Hendriks & the D2TM Team
 * @www http://www.dune2themaker.com
 * @copyright Copyright (c) 2001 - 2026 D2TM Team
 * @license This software is released under the MIT License.
 *
 * Note: Dune 2 is a trademark of Westwood Studios/Electronic Arts.
 *
 * This is a non-commercial educational project.
 */

#pragma once

#include "cGameState.h"
#include "controls/cKeyboardEvent.h"
#include "utils/cRectangle.h"
#include "controls/sMouseEvent.h"

#include <memory>

class Texture;
class cTextDrawer;
class cGameInterface;
class GuiButton;
class GuiWindow;

class cMainMenuState : public cGameState {
public:
    explicit cMainMenuState(sGameServices* services);
    ~cMainMenuState() override;

    void thinkFast() override;
    void draw() const override;

    void onNotifyMouseEvent(const s_MouseEvent &event) override;
    void onNotifyKeyboardEvent(const cKeyboardEvent &event) override;

    eGameStateType getType() override;

private:
    cGameSettings * m_settings = nullptr;
    cTextDrawer* m_textDrawer = nullptr;
    cGameInterface* m_interface = nullptr;    

    int logoX;
    int logoY;

    int mainMenuFrameX;
    int mainMenuFrameY;
    int mainMenuWidth;
    int mainMenuHeight;

    std::unique_ptr<GuiWindow> gui_window;
    std::unique_ptr<GuiButton> gui_btn_credits;

    Texture *bmp_D2TM_Title;
    Texture *backGroundDebug = nullptr;
};
