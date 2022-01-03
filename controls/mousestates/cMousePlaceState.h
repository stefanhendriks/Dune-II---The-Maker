#pragma once

#include "cMouseState.h"

class cBuildingListItem;

/**
 * Mouse placing structure at battlefield
 */
class cMousePlaceState : public cMouseState {

public:
    explicit cMousePlaceState(cPlayer * player, cGameControlsContext *context, cMouse * mouse);

    void onNotifyMouseEvent(const s_MouseEvent &event) override;
    void onNotifyKeyboardEvent(const s_KeyboardEvent &event) override;
    void onStateSet() override;

    void onFocus() override {};
    void onBlur() override {};

private:
    void onMouseLeftButtonClicked();

    void onMouseRightButtonPressed();

    void onMouseRightButtonClicked();

    void onMouseMovedTo();

    bool mayPlaceIt(cBuildingListItem *itemToPlace, int mouseCell);
};
