#ifndef D2TM_CMOUSENORMALSTATE_H
#define D2TM_CMOUSENORMALSTATE_H

#include "cMouseState.h"
#include "controls/cMouse.h"
#include "sMouseEvent.h"
#include "sKeyboardEvent.h"

class cPlayer;

enum eMouseNormalState {
    SELECT_STATE_NORMAL, // normal state, selecting stuff, etc
    SELECT_STATE_RALLY,  // set rally point for structure
};

static const char* mouseNormalStateString(const eMouseNormalState &state) {
    switch (state) {
        case SELECT_STATE_NORMAL: return "SELECT_STATE_NORMAL";
        case SELECT_STATE_RALLY: return "SELECT_STATE_RALLY";
        default:
            assert(false);
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
    explicit cMouseNormalState(cPlayer * player, cGameControlsContext *context, cMouse * mouse);

    void onNotifyMouseEvent(const s_MouseEvent &event) override;
    void onNotifyKeyboardEvent(const s_KeyboardEvent &event) override;

    void onStateSet() override;

private:
    void onMouseLeftButtonClicked(const s_MouseEvent &event);

    void onMouseRightButtonPressed(const s_MouseEvent &event);

    void onMouseRightButtonClicked(const s_MouseEvent &event);

    void onMouseMovedTo(const s_MouseEvent &event);

    void onKeyDown(const s_KeyboardEvent &event);

    void onKeyPressed(const s_KeyboardEvent &event);

    eMouseNormalState state;

    int getMouseTileForNormalState() const;
};


#endif //D2TM_CMOUSENORMALSTATE_H
