#pragma once

// this class holds the game controls context. This means, the updateMouseAndKeyboardStateAndGamePlaying() method
// will check how the mouse is positioned in this particular frame. It then updates
// any ids, or other relevant data.
//
// For instance, this class knows if a mouse is hovering above a structure, or a unit. The sidebar
// or above the minimap. It also knows if an order has been issued (attack, move, repair, etc).
//
// a context belongs to a player


// 01/01/2022 -> Move this into a `Combat` state object; or within player object as state.

#include "gameobjects/structures/cAbstractStructure.h"
#include "include/definitions.h"

#include "controls/mousestates/eMouseStates.h"
#include "controls/mousestates/cMouseNormalState.h"
#include "controls/mousestates/cMouseUnitsSelectedState.h"
#include "controls/mousestates/cMouseRepairState.h"
#include "controls/mousestates/cMousePlaceState.h"
#include "controls/mousestates/cMouseDeployState.h"

class cGameControlsContext : public cInputObserver, cScenarioObserver {
public:
    explicit cGameControlsContext(cPlayer *player, cMouse *mouse);
    ~cGameControlsContext() override;

    int getIdOfStructureWhereMouseHovers() const {
        return m_mouseHoveringOverStructureId;
    }
    int getIdOfUnitWhereMouseHovers() const {
        return m_mouseHoveringOverUnitId;
    }

    int getMouseCell() const {
        return m_mouseCell;
    }

    bool isMouseOverStructure() const {
        return m_mouseHoveringOverStructureId > -1;
    }
    bool isMouseOverUnit() const {
        return m_mouseHoveringOverUnitId > -1;
    }

    bool isMouseOnSidebar() const {
        return m_mouseCell == MOUSECELL_SIDEBAR;
    }
    bool isMouseOnTopBar() const {
        return m_mouseCell == MOUSECELL_TOPBAR;
    }
    bool isMouseOnMiniMap() const {
        return m_mouseCell == MOUSECELL_MINIMAP;
    }
    bool isMouseOnSidebarOrMinimap() const {
        return isMouseOnSidebar() || isMouseOnMiniMap();
    }
    bool isMouseOnBattleField() const;

    bool shouldDrawToolTip() const {
        return m_drawToolTip;
    }
    bool isMouseRightButtonPressed() const{
        return m_mouse->isRightButtonPressed();
    }

    cAbstractStructure *getStructurePointerWhereMouseHovers() const;

    int getMouseCellFromScreen(int mouseX, int mouseY) const;

    void onNotifyMouseEvent(const s_MouseEvent &event) override;
    void onNotifyKeyboardEvent(const cKeyboardEvent &event) override;
    void onNotifyGameEvent(const s_GameEvent &event) override;

    void setMouseState(eMouseState newState);

    void toPreviousState();

    bool isState(eMouseState other) const;

    void onFocusMouseStateEvent();

protected:
    void determineToolTip();
    void determineHoveringOverStructureId();
    void determineHoveringOverUnitId();

private:
    int m_mouseHoveringOverStructureId;
    int m_mouseHoveringOverUnitId;
    bool m_mouseOnBattleField;

    bool m_drawToolTip;

    // on what cell is the mouse hovering
    int m_mouseCell;

    // context belongs to specific player
    cPlayer *m_player;

    // the state to direct events to
    eMouseState m_state;
    eMouseState m_prevState;             // used to switch back previous state (ie from Place, or Repair mode)
    eMouseState m_prevStateBeforeRepair; // the state before repair mode only, used when Place/Repair mode is used
    // in conjunction

    cMouse *m_mouse;

    // the states, initialized once to save a lot of construct/destructs (and make it possible
    // to switch between states without needing to restore 'state' within the state object)
    cMouseNormalState *m_mouseNormalState;
    cMouseUnitsSelectedState *m_mouseUnitsSelectedState;
    cMouseRepairState *m_mouseRepairState;
    cMousePlaceState *m_mousePlaceState;
    cMouseDeployState *m_mouseDeployState;

    //
    bool m_prevTickMouseAtBattleField;

    // mouse state
    void onNotifyMouseStateEvent(const s_MouseEvent &event);
    void onBlurMouseStateEvent();
    void onMouseMovedTo(const s_MouseEvent &event);
    void updateMouseCell(const cPoint &coords);
};