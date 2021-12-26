#ifndef CGAMECONTROLSCONTEXT_H_
#define CGAMECONTROLSCONTEXT_H_

// this class holds the game controls context. This means, the updateState() method
// will check how the mouse is positioned in this particular frame. It then updates
// any ids, or other relevant data.
//
// For instance, this class knows if a mouse is hovering above a structure, or a unit. The sidebar
// or above the minimap. It also knows if an order has been issued (attack, move, repair, etc).
//
// a context belongs to a player


// 24/12/2021 -> This is a kind of "Combat Mouse State" object really. This could move into a
// combat state object, or 2 separate objects (state & mouse object).

// Mouse status (taken from somewhere else)
//#define MOUSE_STATE_NORMAL  0
//#define MOUSE_STATE_MOVE    1
//#define MOUSE_STATE_ATTACK  2
//#define MOUSE_STATE_PLACE   3

#include "controls/mousestates/cMouseNormalState.h"

enum eMouseState {
    MOUSESTATE_SELECT, // mouse is in 'normal' mode, it is able to select units
    MOUSESTATE_UNITS_SELECTED, // mouse has selected units
    MOUSESTATE_REPAIR, // mouse is in repair state
    MOUSESTATE_PLACE, // mouse is going to place a structure
};

static const char* mouseStateString(const eMouseState &state) {
    switch (state) {
        case MOUSESTATE_SELECT: return "MOUSESTATE_SELECT";
        case MOUSESTATE_UNITS_SELECTED: return "MOUSESTATE_UNITS_SELECTED";
        case MOUSESTATE_REPAIR: return "MOUSESTATE_REPAIR";
        case MOUSESTATE_PLACE: return "MOUSESTATE_PLACE";
        default:
            assert(false);
            break;
    }
    return "";
}


class cGameControlsContext : public cInputObserver {
	public:
		cGameControlsContext(cPlayer *thePlayer);
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
        void onNotifyKeyboardEvent(const s_KeyboardEvent &event) override;

        void setMouseState(eMouseState newState);

        void updateMouseState();

    protected:
		void determineToolTip();
		void determineHoveringOverStructureId(int mouseX, int mouseY);
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

        // the states, initialized once to save a lot of construct/destructs
        cMouseNormalState * mouseNormalState;

};

#endif /* CGAMECONTROLSCONTEXT_H_ */
