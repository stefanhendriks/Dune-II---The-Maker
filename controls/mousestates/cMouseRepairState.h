#ifndef D2TM_CMOUSEREPAIRSTATE_H
#define D2TM_CMOUSEREPAIRSTATE_H

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

private:
    void onMouseLeftButtonClicked(const s_MouseEvent &event);

    void onMouseRightButtonPressed(const s_MouseEvent &event);

    void onMouseRightButtonClicked(const s_MouseEvent &event);

    void onMouseMovedTo(const s_MouseEvent &event);

    int getMouseTileForRepairState();

};


#endif //D2TM_CMOUSEREPAIRSTATE_H
