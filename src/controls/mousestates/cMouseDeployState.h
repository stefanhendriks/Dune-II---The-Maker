/**
 * @file cMouseDeployState.h
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

#include "controls/cKeyboardEvent.h"
#include "controls/cMouse.h"
#include "cMouseState.h"
#include "sGameEvent.h"
#include "controls/sMouseEvent.h"

class cGameControlsContext;
class cPlayer;

/**
 * Mouse placing structure at battlefield
 */
class cMouseDeployState : public cMouseState {

public:
    explicit cMouseDeployState(cPlayer *player, cGameControlsContext *context, cMouse *mouse);

    void onNotifyMouseEvent(const s_MouseEvent &event) override;
    void onNotifyKeyboardEvent(const cKeyboardEvent &event) override;
    void onNotifyGameEvent(const s_GameEvent &) override;

    void onStateSet() override;

    void onFocus() override;
    void onBlur() override;

private:
    void onMouseLeftButtonClicked();

    void onMouseRightButtonPressed();

    void onMouseRightButtonClicked();

    void onMouseMovedTo();
};
