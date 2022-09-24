#pragma once

#include "controls/cKeyboardEvent.h"
#include "controls/cMouse.h"
#include "cMouseState.h"
#include "sGameEvent.h"
#include "sMouseEvent.h"

#include <vector>

class cGameControlsContext;
class cPlayer;

enum eMouseUnitsSelectedState {
    SELECTED_STATE_MOVE, // move selected units to location
    SELECTED_STATE_SELECT, // select a structure
    SELECTED_STATE_ADD_TO_SELECTION, // add units to current selection
    SELECTED_STATE_ATTACK, // attack target with selected units
    SELECTED_STATE_FORCE_ATTACK, // force attack target with selected units
    SELECTED_STATE_REPAIR, // enter structure to repair units
    SELECTED_STATE_CAPTURE, // enter structure to capture it
    SELECTED_STATE_REFINERY, // enter refinery to dump spice / collect credits
};

/**
 * A mouse "units selected" state is at the battlefield, and it is active when units have been selected (box, or single).
 * This state basically determines what kind of actions can be performed by the selected units. Ie, attacking an enemy
 * structure/unit. Or send (a part of) the selected units to dump spice in refinery, or get repaired.
 *
 * This state object has its own "state" to tell what kind of action is applicable, even though the rendered
 * mouseTile might be the same. Ie, there is (for now) no different mouseTile rendered when a unit can enter a
 * structure or when it is a 'normal move order'.
 *
 */
class cMouseUnitsSelectedState : public cMouseState {

public:
    explicit cMouseUnitsSelectedState(cPlayer * player, cGameControlsContext *context, cMouse * mouse);

    void onNotifyMouseEvent(const s_MouseEvent &event) override;
    void onNotifyKeyboardEvent(const cKeyboardEvent &event) override;
    void onNotifyGameEvent(const s_GameEvent &) override {}

    void onStateSet() override;

    void onFocus() override;
    void onBlur() override;

private:
    // A list of unit id's that we have selected
    std::vector<int> m_selectedUnits;

    // when move to refinery, it will only be applicable to harvesters.
    bool m_harvestersSelected;

    // todo: when we want to capture structures, it will only send out the infantry
    bool m_infantrySelected;

    // when sending to repair facility, only repairable units go to that
    bool m_repairableUnitsSelected;

    eMouseUnitsSelectedState m_state;
    eMouseUnitsSelectedState m_prevState;

    void onMouseLeftButtonClicked();

    void onMouseRightButtonPressed();

    void onMouseRightButtonClicked();

    void onMouseMovedTo();

    void updateSelectedUnitsState();

    void evaluateSelectedUnits();

    void setState(eMouseUnitsSelectedState newState);

    void onKeyDown(const cKeyboardEvent &event);

    void onKeyPressed(const cKeyboardEvent &event);

    void spawnParticle(const int attack);

    void toPreviousState();

    void evaluateMouseMoveState();
};
