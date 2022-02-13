#pragma once

#include <vector>
#include "observers/cInputObserver.h"
#include "utils/cPoint.h"
#include "drawers/cTextDrawer.h"

#include <string>

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

    void setTile(int value);

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

    void addDebugLine(std::string basicString);

private:
    cInputObserver *_mouseObserver;

    cTextDrawer m_textDrawer;

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

    std::vector<std::string> debugLines;

    std::string mouseTileName(int tile);

};
