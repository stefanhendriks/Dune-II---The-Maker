#ifndef CPLACEITDRAWER_H_
#define CPLACEITDRAWER_H_

class cPlaceItDrawer {
public:
    cPlaceItDrawer(cPlayer *thePlayer);

    ~cPlaceItDrawer();

    void draw(cBuildingListItem *itemToPlace, int mouseCell);

protected:
    void drawStructureIdAtMousePos(cBuildingListItem *itemToPlace);

    void drawStatusOfStructureAtCell(cBuildingListItem *itemToPlace, int mouseCell);

private:
    cStructureUtils structureUtils;

    cPlayer *player;
};

#endif /* CPLACEITDRAWER_H_ */
