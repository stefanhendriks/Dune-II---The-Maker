/**
 * @file cWinLoseState.h
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
#include <memory>

class cGameSettings;
class GameStateTransition;
class cMouse;
class Graphics;
class Texture;

class cVideoState : public cGameState {
public:
    explicit cVideoState(sGameServices* services);
    ~cVideoState() override;

    void thinkFast() override;
    void thinkNormal() override;
    void draw() const override;

    void onNotifyMouseEvent(const s_MouseEvent &event) override;
    void onNotifyKeyboardEvent(const cKeyboardEvent &event) override;

    eGameStateType getType() override;

private:
    cGameSettings* m_settings = nullptr;
    GameStateTransition* m_stateTransitions = nullptr;
    cMouse* m_mouse = nullptr;
    std::unique_ptr<Graphics> gfxmovie;
    int m_movieFrame = 0;
    int offsetX;
    int offsetY;
    Texture* m_currentFrame = nullptr;
};

