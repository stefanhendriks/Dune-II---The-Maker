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
		cGameControlsContext(cPlayer *thePlayer, cMouse *theMouse);
		~cGameControlsContext();

		int getIdOfStructureWhereMouseHovers() const { return mouseHoveringOverStructureId; }
		int getIdOfUnitWhereMouseHovers() const { return mouseHoveringOverUnitId; }

		int getMouseCell() const { return mouseCell; }

		bool isMouseOverStructure() { return mouseHoveringOverStructureId > -1; }
		bool isMouseOverUnit() { return mouseHoveringOverUnitId > -1; }

		bool isMouseOnSidebar() { return mouseCell == MOUSECELL_SIDEBAR; }
		bool isMouseOnTopBar() { return mouseCell == MOUSECELL_TOPBAR; }
		bool isMouseOnMiniMap() { return mouseCell == MOUSECELL_MINIMAP; }
        bool isMouseOnSidebarOrMinimap() { return isMouseOnSidebar() || isMouseOnMiniMap(); }
		bool isMouseOnBattleField() { return mouseCell > -1; }

		bool shouldDrawToolTip() { return drawToolTip; }

		cAbstractStructure * getStructurePointerWhereMouseHovers();

		int getMouseCellFromScreen(int mouseX, int mouseY) const;

        void onNotifyMouseEvent(const s_MouseEvent &event) override;
        void onNotifyKeyboardEvent(const cKeyboardEvent &event) override;
        void onNotifyGameEvent(const s_GameEvent &event) override;

        void setMouseState(eMouseState newState);

        void toPreviousState();

        bool isState(eMouseState other);

	protected:
		void determineToolTip();
		void determineHoveringOverStructureId();
		void determineHoveringOverUnitId();

	private:
        void onMouseMovedTo(const s_MouseEvent &event);

        void updateMouseCell(const cPoint &coords);

		int mouseHoveringOverStructureId;
		int mouseHoveringOverUnitId;

		bool drawToolTip;

		// on what cell is the mouse hovering
		int mouseCell;

		// context belongs to specific player
		cPlayer * player;

        // the state to direct events to
        eMouseState state;
        eMouseState prevState; // in case we want to switch from repair mode (back and forth)

        // the states, initialized once to save a lot of construct/destructs (and make it possible
        // to switch between states without needing to restore 'state' within the state object)
        cMouseNormalState * mouseNormalState;
        cMouseUnitsSelectedState * mouseUnitsSelectedState;
        cMouseRepairState * mouseRepairState;
		cMousePlaceState * mousePlaceState;
		cMouseDeployState * mouseDeployState;

		//
		bool prevTickMouseAtBattleField;

		// mouse state
		void onNotifyMouseStateEvent(const s_MouseEvent &event);
		void onFocusMouseStateEvent();
		void onBlurMouseStateEvent();

		cMouse *mouse;
};