/**
 * @file cMouseNormalState.h
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

#include "cMouseState.h"
#include "controls/cMouse.h"
#include "controls/sMouseEvent.h"
#include "controls/cKeyboardEvent.h"

class cPlayer;

enum eMouseNormalState {
    SELECT_STATE_NORMAL, // normal state, selecting stuff, etc
    SELECT_STATE_RALLY,  // set rally point for structure
};

inline const char *mouseNormalStateString(const eMouseNormalState &state)
{
    switch (state) {
        case SELECT_STATE_NORMAL:
            return "SELECT_STATE_NORMAL";
        case SELECT_STATE_RALLY:
            return "SELECT_STATE_RALLY";
        default:
            d2tm_assert(false);
            break;
    }
    return "";
}

/**
 * A mouse normal state is at the battlefield, and is the default state the mouse is in. It can select units, drag
 * a border (for multi-select) and drag the viewport
 *
 */
class cMouseNormalState : public cMouseState {

public:
    explicit cMouseNormalState(cPlayer *player, cGameControlsContext *context, cMouse *mouse);

    void onNotifyMouseEvent(const s_MouseEvent &event) override;
    void onNotifyKeyboardEvent(const cKeyboardEvent &event) override;
    void onNotifyGameEvent(const s_GameEvent &) override {}

    void onStateSet() override;

    void onFocus() override;
    void onBlur() override;

private:
    eMouseNormalState m_state;

    void onMouseLeftButtonClicked();

    void onMouseRightButtonPressed();

    void onMouseRightButtonClicked();

    void onMouseMovedTo();

    void onKeyDown(const cKeyboardEvent &event);

    void onKeyPressed(const cKeyboardEvent &event);

    void setState(eMouseNormalState newState);

    int getMouseTileForNormalState() const;
};
