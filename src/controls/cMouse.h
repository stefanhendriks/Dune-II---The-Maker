#pragma once

#include "utils/cPoint.h"

#include <SDL2/SDL.h>
#include <string>

class cRectangle;
class GameContext;
class SDLDrawer;
class cInputObserver;

class cMouse {

public:
    cMouse(GameContext *ctx);

    ~cMouse();

    void handleEvent(const SDL_Event &event);

    void updateState(); // updates state from Allegro, calls appropriate on* methods on gameControlContext class

    void setMouseObserver(cInputObserver *mouseObserver);

    void init();

    // these functions return true when the mouse button is being hold down
    bool isLeftButtonPressed() {
        return m_leftButtonPressed;
    }

    bool isRightButtonPressed() {
        return m_rightButtonPressed;
    }

    // these functions return true when the mouse button was pressed in the previous
    // frame, but released in the current (which is counted as a 'click')
    bool isLeftButtonClicked() {
        return m_leftButtonClicked;
    }

    bool isRightButtonClicked() {
        return m_rightButtonClicked;
    }

    int getX() {
        return m_coords.x;
    }

    int getY() {
        return m_coords.y;
    }

    void setCursorPosition(SDL_Window *_windows, int x, int y);

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

    cPoint getMouseCoords();

    cPoint getDragLineStartPoint();

    cPoint getDragLineEndPoint();

private:
    cInputObserver *m_mouseObserver = nullptr;
    GameContext* m_ctx = nullptr;
    SDLDrawer* m_renderDrawer = nullptr;

    bool m_leftButtonPressed;
    bool m_rightButtonPressed;

    bool m_leftButtonReleased;
    bool m_rightButtonReleased;

    bool m_leftButtonClicked;
    bool m_rightButtonClicked;

    bool m_leftButtonClickedInPreviousFrame;
    bool m_rightButtonClickedInPreviousFrame;

    bool m_mouseScrolledUp;
    bool m_mouseScrolledDown;
    bool m_didMouseMove;

    cPoint m_coords;
    cPoint m_coordsOnClick;

    // Mouse information - for select box and dragging, etc
    int m_mouseCoX1;        // coordinates
    int m_mouseCoY1;        // of
    int m_mouseCoX2;        // the
    int m_mouseCoY2;        // select box

    int m_mouseMvX1;        // coordinates
    int m_mouseMvY1;        // of
    int m_mouseMvX2;        // the
    int m_mouseMvY2;        // dragging viewport

    int m_mouseTile;       // mouse picture in gfxdata

    std::string mouseTileName(int tile);

};
