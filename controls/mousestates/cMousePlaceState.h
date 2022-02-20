#pragma once

#include "controls/cKeyboardEvent.h"
#include "controls/cMouse.h"
#include "cMouseState.h"
#include "sGameEvent.h"
#include "sidebar/cBuildingListItem.h"
#include "sMouseEvent.h"

class cGameControlsContext;
class cPlayer;

/**
 * Mouse placing structure at battlefield
 */
class cMousePlaceState : public cMouseState {

public:
    explicit cMousePlaceState(cPlayer * player, cGameControlsContext *context, cMouse * mouse);

    void onNotifyMouseEvent(const s_MouseEvent &event) override;
    void onNotifyKeyboardEvent(const cKeyboardEvent &event) override;
    void onNotifyGameEvent(const s_GameEvent &) override {}

    void onStateSet() override;

    void onFocus() override;
    void onBlur() override {}

private:
    void onMouseLeftButtonClicked();

    void onMouseRightButtonPressed();

    void onMouseRightButtonClicked();

    void onMouseMovedTo();

    bool mayPlaceIt(cBuildingListItem *itemToPlace, int mouseCell);
};
