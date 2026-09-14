/**
 * @file cSelectMissionState.h
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
#include "controls/sMouseEvent.h"

#include <memory>

struct SDL_Surface;

class GameContext;
class cTextDrawer;
class GuiWindow;
class cGameSettings;
class cGameInterface;

class cSelectMissionState : public cGameState {
public:
    cSelectMissionState(sGameServices* services, int prevState);
    ~cSelectMissionState() override;

    void thinkFast() override;
    void draw() const override;

    void onNotifyMouseEvent(const s_MouseEvent &event) override;
    void onNotifyKeyboardEvent(const cKeyboardEvent &event) override;

    eGameStateType getType() override;

private:
    cTextDrawer *m_textDrawer = nullptr;
    cGameSettings* m_settings = nullptr;
    cGameInterface* m_interface = nullptr;
    std::unique_ptr<GuiWindow> gui_window;
};
