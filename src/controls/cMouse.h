/**
 * @file cMouse.h
 *
 * Dune 2 - The Maker
 *
 * @author Stefan Hendriks & the D2TM Team
 * @www http://www.dune2themaker.com
 * @copyright Copyright (c) 2002 - 2026 Stefan Hendriks and contributors
 * @license This software is released under the MIT License. See LICENSE.md.
 *
 * Note: Dune 2 is a trademark of Westwood Studios/Electronic Arts.
 *
 * This is a non-commercial educational project.
 */

#pragma once

#include "controls/eMouseTile.h"
#include "utils/GraphicsID.hpp"
#include "utils/cPoint.h"

#include <SDL3/SDL.h>
#include <array>
#include <string>

class cRectangle;
class GameContext;
class SDLDrawer;
class cInputObserver;
class cGameSettings;

class cMouse {

public:
    cMouse(GameContext *ctx);

    ~cMouse();

    void handleEvent(const SDL_Event &event);

    void updateState(); // updates state from SDL, calls appropriate on* methods on gameControlContext class

    void setMouseObserver(cInputObserver *mouseObserver);

    void init();

    void setSettings(cGameSettings *settings) { m_settings = settings; }

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

    // Injection API for the gamepad, in render coordinates. Sets the same state as real mouse events.
    void moveTo(int x, int y);
    void setLeftButton(bool down);
    void setRightButton(bool down);
    void scroll(bool up);

    // true when the cursor was last moved by the gamepad, false again once the real mouse moves
    bool isDrivenByPad() const { return m_padDriven; }

    void setCursorPosition(SDL_Window *_windows, int x, int y);

    bool isOverRectangle(int x, int y, int width, int height);

    bool isOverRectangle(cRectangle *rectangle);

    bool isMapScrolling();

    bool isBoxSelecting();

    void setTile(eMouseTile value);

    void resetDragViewportInteraction();

    void dragViewportInteraction();

    void boxSelectLogic(int mouseCell);

    cRectangle getBoxSelectRectangle();

    void resetBoxSelect();

    bool isTile(eMouseTile value);

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
    SDLDrawer* m_sdlDrawer = nullptr;
    cGameSettings* m_settings = nullptr;

    bool m_leftButtonPressed;
    bool m_rightButtonPressed;

    bool m_leftButtonReleased;
    bool m_rightButtonReleased;

    bool m_leftButtonClicked;
    bool m_rightButtonClicked;
    bool m_leftButtonDoubleClicked;

    bool m_leftButtonClickedInPreviousFrame;
    bool m_rightButtonClickedInPreviousFrame;

    bool m_mouseScrolledUp;
    bool m_mouseScrolledDown;
    bool m_didMouseMove;

    cPoint m_coords;
    cPoint m_coordsOnClick;

    // gamepad injection state
    bool m_padDriven = false;
    bool m_padLeftHeld = false;
    bool m_padRightHeld = false;
    bool m_padLeftIsDoubleClick = false;
    Uint64 m_padLastLeftClickTicks = 0;

    // Mouse information - for select box and dragging, etc
    int m_mouseCoX1;        // coordinates
    int m_mouseCoY1;        // of
    int m_mouseCoX2;        // the
    int m_mouseCoY2;        // select box

    int m_mouseMvX1;        // coordinates
    int m_mouseMvY1;        // of
    int m_mouseMvX2;        // the
    int m_mouseMvY2;        // dragging viewport

    eMouseTile m_mouseTile;       // mouse picture in gfxdata
    // the sprite of every eMouseTile, asked from the graphics pack once (on first draw)
    std::array<TextureID, 11> m_tileTextures;
    bool m_tileTexturesResolved = false;

    void resolveTileTextures();
    std::string mouseTileName(eMouseTile tile);

};
