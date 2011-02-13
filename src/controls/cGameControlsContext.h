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

		void updateState();

		int getIdOfStructureWhereMouseHovers() { return mouseHoveringOverStructureId; }
		int getIdOfUnitWhereMouseHovers() { return mouseHoveringOverUnitId; }

		int getMouseCell() { return  mouseCell; }
		int getMouseCellFromMiniMap();

		bool isMouseOverStructure() { return mouseHoveringOverStructureId > -1; }
		bool isMouseOverUnit() { return mouseHoveringOverUnitId > -1; }

		bool isMouseOnSidebar() { return mouseCell == -3; }
		bool isMouseOnTopBar() { return mouseCell == -1; }
		bool isMouseOnMiniMap() { return mouseCell == -2; }
		bool isMouseOnBattleField() { return mouseCell > -1; }

		bool shouldDrawToolTip() { return drawToolTip; }

		cAbstractStructure * getStructurePointerWhereMouseHovers();

	protected:
		void determineMouseCell();
		void determineToolTip();
		void determineHoveringOverStructureId();
		void determineHoveringOverUnitId();

	private:
		int mouseHoveringOverStructureId;
		int mouseHoveringOverUnitId;

		bool drawToolTip;

		// on what cell is the mouse hovering
		int mouseCell;

		// context belongs to specific player
		cPlayer * player;
		cMouse * mouse;
		cCellCalculator * cellCalculator;

};

#endif /* CGAMECONTROLSCONTEXT_H_ */
