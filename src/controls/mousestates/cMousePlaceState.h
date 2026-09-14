/**
 * @file cMousePlaceState.h
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

#include "controls/cKeyboardEvent.h"
#include "controls/cMouse.h"
#include "cMouseState.h"
#include "sGameEvent.h"
#include "sidebar/cBuildingListItem.h"
#include "controls/sMouseEvent.h"

class cGameControlsContext;
class cPlayer;

/**
 * Mouse placing structure at battlefield
 */
class cMousePlaceState : public cMouseState {

public:
    explicit cMousePlaceState(cPlayer *player, cGameControlsContext *context, cMouse *mouse);

    void onNotifyMouseEvent(const s_MouseEvent &event) override;
    void onNotifyKeyboardEvent(const cKeyboardEvent &event) override;
    void onNotifyGameEvent(const s_GameEvent &) override {}

    void onStateSet() override;

    void onFocus() override;
    void onBlur() override;

private:
    void onMouseLeftButtonClicked();

    void onMouseRightButtonPressed();

    void onMouseRightButtonClicked();

    void onMouseMovedTo();

    bool mayPlaceIt(cBuildingListItem *itemToPlace, int mouseCell);
};
