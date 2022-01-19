#pragma once

#include "cMouseState.h"

class cBuildingListItem;

/**
 * Mouse placing structure at battlefield
 */
class cMouseDeployState : public cMouseState {

public:
    explicit cMouseDeployState(cPlayer * player, cGameControlsContext *context, cMouse * mouse);

    void onNotifyMouseEvent(const s_MouseEvent &event) override;
    void onNotifyKeyboardEvent(const cKeyboardEvent &event) override;
    void onNotifyGameEvent(const s_GameEvent &) override;

    void onStateSet() override;

    void onFocus() override;
    void onBlur() override {}

private:
    void onMouseLeftButtonClicked();

    void onMouseRightButtonPressed();

    void onMouseRightButtonClicked();

    void onMouseMovedTo();
};
