#ifndef CPLACEITDRAWER_H_
#define CPLACEITDRAWER_H_

class cPlaceItDrawer {
	public:
		cPlaceItDrawer(cPlayer * thePlayer);
		~cPlaceItDrawer();

		void draw(cBuildingListItem *itemToPlace);

    void onMouseClickedLeft(int x, int y);

protected:
		void drawStructureIdAtCell(cBuildingListItem *itemToPlace, int cell);
		void drawStatusOfStructureAtCell(cBuildingListItem *itemToPlace, int mouseCell);

	private:

        bool bWithinBuildDistance;
        bool bMayPlace;
        int iTotalRocks;
        int iTotalBlocks;

        cStructureUtils structureUtils;

        cPlayer * m_Player;
        cBuildingListItem *itemToPlace;
};

#endif /* CPLACEITDRAWER_H_ */
