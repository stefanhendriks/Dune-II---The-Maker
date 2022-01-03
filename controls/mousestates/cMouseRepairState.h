#pragma once

#include "cMouseState.h"

/**
 * In this state the mouse can repair (or order to repair) things.
 *
 */
class cMouseRepairState : public cMouseState {

public:
    explicit cMouseRepairState(cPlayer * player, cGameControlsContext *context, cMouse * mouse);

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

    int getMouseTileForRepairState();

};
