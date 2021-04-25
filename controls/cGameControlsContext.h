/*
 * cGameControlsContext.h
 *
 *  Created on: 31-okt-2010
 *      Author: Stefan
 */

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

class cGameControlsContext {
	public:
		cGameControlsContext(cPlayer *thePlayer);
		~cGameControlsContext();

		void onMouseAt(int x, int y);

		int getIdOfStructureWhereMouseHovers() { return mouseHoveringOverStructureId; }
		int getIdOfUnitWhereMouseHovers() { return mouseHoveringOverUnitId; }

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

	protected:
		void determineToolTip();
		void determineHoveringOverStructureId(int mouseX, int mouseY);
		void determineHoveringOverUnitId();


	private:
        void updateMouseCell(int mouseX, int mouseY);

		int mouseHoveringOverStructureId;
		int mouseHoveringOverUnitId;

		bool drawToolTip;

		// on what cell is the mouse hovering
		int mouseCell;

		// context belongs to specific player
		cPlayer * player;
};

#endif /* CGAMECONTROLSCONTEXT_H_ */
