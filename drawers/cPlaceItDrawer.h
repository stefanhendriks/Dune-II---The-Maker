#ifndef CPLACEITDRAWER_H_
#define CPLACEITDRAWER_H_

class cPlaceItDrawer {
public:
    cPlaceItDrawer(cPlayer *thePlayer);

    ~cPlaceItDrawer();

    void draw(cBuildingListItem *itemToPlace);

    void onNotify(const s_MouseEvent &event);

protected:
    void drawStructureIdAtCell(cBuildingListItem *itemToPlace, int cell);

    void drawStatusOfStructureAtCell(cBuildingListItem *itemToPlace, int mouseCell);

private:
    void onMouseClickedLeft(const s_MouseEvent &event);

    bool m_bMayPlace;

    cStructureUtils structureUtils;

    cPlayer *player;
    cBuildingListItem *itemToPlace;
};

#endif /* CPLACEITDRAWER_H_ */
