#ifndef CMOUSE_H_
#define CMOUSE_H_

#include <utils/cPoint.h>
#include "../observers/cInputObserver.h"

class cRectangle;

class cMouse {

public:
    cMouse();

    ~cMouse();

    void updateState(); // updates state from Allegro, calls appropriate on* methods on gameControlContext class

    void setMouseObserver(cInputObserver *mouseObserver) {
        this->_mouseObserver = mouseObserver;
    }

    void init();

    // these functions return true when the mouse button is being hold down
    bool isLeftButtonPressed() { return leftButtonPressed; }

    bool isRightButtonPressed() { return rightButtonPressed; }

    // these functions return true when the mouse button was pressed in the previous
    // frame, but released in the current (which is counted as a 'click')
    bool isLeftButtonClicked() { return leftButtonClicked; }

    bool isRightButtonClicked() { return rightButtonClicked; }

    int getX() { return coords.x; }

    int getY() { return coords.y; }

    int getZ() { return z; }

    void positionMouseCursor(int x, int y);

    bool isOverRectangle(int x, int y, int width, int height);

    bool isOverRectangle(cRectangle *rectangle);

    bool isMapScrolling();

    bool isBoxSelecting();

    /**
     * Sets tile of gfx
     * TODO: should become "state" so it decouples drawing and game mechanic
     * @param value
     */
    void setTile(int value) { mouse_tile = value; }

    void resetDragViewportInteraction();

    void dragViewportInteraction();

    void boxSelectLogic(int mouseCell);

    cRectangle getBoxSelectRectangle();

    void resetBoxSelect();

    bool isTile(int value);

    void draw();

    bool isNormalRightClick();

    int getMouseDragDeltaX();

    int getMouseDragDeltaY();

    cPoint getDragLineStartPoint();

    cPoint getDragLineEndPoint();

private:
    cInputObserver *_mouseObserver;

    bool leftButtonPressed;
    bool rightButtonPressed;

    bool leftButtonClicked;
    bool rightButtonClicked;

    bool leftButtonPressedInPreviousFrame;
    bool rightButtonPressedInPreviousFrame;

    bool mouseScrolledUp;
    bool mouseScrolledDown;

    cPoint coords;
    int z;    // z = scroll wheel value
    int zValuePreviousFrame;

    // Mouse information - for select box and dragging, etc
    int mouse_co_x1;        // coordinates
    int mouse_co_y1;        // of
    int mouse_co_x2;        // the
    int mouse_co_y2;        // select box

    int mouse_mv_x1;        // coordinates
    int mouse_mv_y1;        // of
    int mouse_mv_x2;        // the
    int mouse_mv_y2;        // dragging viewport

    int mouse_tile;       // mouse picture in gfxdata
};

#endif /* CMOUSE_H_ */
