#ifndef CORDERDRAWER_H_
#define CORDERDRAWER_H_

class cOrderDrawer {
public:
    cOrderDrawer(cPlayer *thePlayer);

    ~cOrderDrawer();

    void drawOrderButton(cPlayer *thePlayer);

    void drawOrderPlaced();

    void drawRectangleOrderButton();

    void onNotify(const s_MouseEvent &event);

    void setPlayer(cPlayer *pPlayer);

protected:

private:
    void onMouseAt(const s_MouseEvent &event);
    void onMouseClickedLeft(const s_MouseEvent &event);

    bool _isMouseOverOrderButton;

    cPlayer * player;
    cRectangle *buttonRect;
    BITMAP *buttonBitmap;

};

#endif /* CORDERDRAWER_H_ */
