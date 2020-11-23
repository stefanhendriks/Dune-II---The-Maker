#ifndef CPLACEITDRAWER_H_
#define CPLACEITDRAWER_H_

class cPlaceItDrawer {
	public:
		cPlaceItDrawer(const cPlayer& thePlayer);
		~cPlaceItDrawer();

		void draw(cBuildingListItem *itemToPlace);

	protected:
		void drawStructureIdAtCell(cBuildingListItem *itemToPlace, int cell);
		void drawStatusOfStructureAtCell(cBuildingListItem *itemToPlace, int mouseCell);

	private:

		cCellCalculator * cellCalculator;
        const cPlayer& m_Player;
};

#endif /* CPLACEITDRAWER_H_ */
