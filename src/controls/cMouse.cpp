#include "cMouse.h"

#include "controls/cGameControlsContext.h"
#include "observers/cInputObserver.h"
#include "game/cGame.h"
#include "include/d2tmc.h"
#include "data/gfxdata.h"
#include "drawers/SDLDrawer.hpp"
#include "drawers/cTextDrawer.h"
#include "player/cPlayer.h"
#include "player/cPlayers.h"
#include "sidebar/cSideBar.h"
#include "utils/d2tm_math.h"
#include "utils/Graphics.hpp"
#include "context/GameContext.hpp"

#include <SDL2/SDL.h>
#include <iostream>
#include <cassert>

cMouse::cMouse(GameContext *ctx) :
    m_ctx(ctx),
    m_textDrawer(ctx->getTextContext()->getBeneTextDrawer()),
    m_renderDrawer(ctx->getSDLDrawer()),
    m_coords(cPoint(0,0))
{
    assert(m_ctx!=nullptr);
    m_leftButtonPressed=false;
    m_rightButtonPressed=false;
    m_leftButtonReleased=false;
    m_rightButtonReleased=false;
    m_mouseScrolledUp=false;
    m_mouseScrolledDown=false;
    m_leftButtonClickedInPreviousFrame = false;
    m_rightButtonClickedInPreviousFrame = false;
    m_mouseObserver = nullptr; // set later
    m_debugLines = std::vector<std::string>();
    m_didMouseMove = false;
    init();
}

cMouse::~cMouse()
{
    m_mouseObserver = nullptr; // we do not own this, so don't delete
}

void cMouse::init()
{
    m_mouseTile = MOUSE_NORMAL;

    m_mouseCoX1 = -1;
    m_mouseCoY1 = -1;
    m_mouseCoX2 = -1;
    m_mouseCoY2 = -1;

    m_mouseMvX1 = -1;
    m_mouseMvY1 = -1;
    m_mouseMvX2 = -1;
    m_mouseMvY2 = -1;

    m_debugLines.clear();
}

void cMouse::handleEvent(const SDL_Event &event)
{
    switch (event.type) {
        case SDL_MOUSEBUTTONDOWN:
            if (event.button.button == SDL_BUTTON_LEFT) {
                m_leftButtonPressed = true;
                m_coordsOnClick.x = event.motion.x;
                m_coordsOnClick.y = event.motion.y;
            }
            if (event.button.button == SDL_BUTTON_RIGHT) {
                m_rightButtonPressed = true;
                m_coordsOnClick.x = event.motion.x;
                m_coordsOnClick.y = event.motion.y;
            }
            break;
        case SDL_MOUSEBUTTONUP:
            if (event.button.button == SDL_BUTTON_LEFT) {
                m_leftButtonPressed = false;
                m_leftButtonReleased = true;
                int dist = (m_coordsOnClick.x - event.motion.x)*(m_coordsOnClick.x - event.motion.x) + (m_coordsOnClick.y - event.motion.y)*(m_coordsOnClick.y - event.motion.y);
                if (dist < 16) {
                    // std::cout << "left click" << std::endl;
                    m_leftButtonClicked = true;
                }
            }
            if (event.button.button == SDL_BUTTON_RIGHT) {
                m_rightButtonPressed = false;
                m_rightButtonReleased = true;
                int dist = (m_coordsOnClick.x - event.motion.x)*(m_coordsOnClick.x - event.motion.x) + (m_coordsOnClick.y - event.motion.y)*(m_coordsOnClick.y - event.motion.y);
                if (dist < 16) {
                    // std::cout << "right click" << std::endl;
                    m_rightButtonClicked = true;
                }
            }
            break;
        case SDL_MOUSEMOTION:
            m_coords.x = event.motion.x;
            m_coords.y = event.motion.y;
            m_didMouseMove = true;
            break;
        case SDL_MOUSEWHEEL:
            if (event.wheel.y > 0) m_mouseScrolledUp = true;
            if (event.wheel.y < 0) m_mouseScrolledDown = true;
            break;
    }
}

void cMouse::updateState()
{
    m_debugLines.clear();

    if (m_leftButtonClickedInPreviousFrame == true)
        m_leftButtonClicked = false;
    if (m_rightButtonClickedInPreviousFrame == true)
        m_rightButtonClicked = false;

    // mouse moved
    if (m_mouseObserver) {
        s_MouseEvent event {eMouseEventType::MOUSE_MOVED_TO, m_coords};

        if (m_didMouseMove) {
            s_MouseEvent event {eMouseEventType::MOUSE_MOVED_TO, m_coords};
            m_mouseObserver->onNotifyMouseEvent(event);
        }

        if (m_mouseScrolledUp) {
            event.eventType = eMouseEventType::MOUSE_SCROLLED_UP;
            m_mouseObserver->onNotifyMouseEvent(event);
        }

        if (m_mouseScrolledDown) {
            event.eventType = eMouseEventType::MOUSE_SCROLLED_DOWN;
            m_mouseObserver->onNotifyMouseEvent(event);
        }

        if (m_leftButtonPressed) {
            event.eventType = eMouseEventType::MOUSE_LEFT_BUTTON_PRESSED;
            m_mouseObserver->onNotifyMouseEvent(event);
            // std::cout << "emit left pressed" << std::endl;
        }

        if (m_leftButtonReleased) {
            event.eventType = eMouseEventType::MOUSE_LEFT_BUTTON_CLICKED;
            m_mouseObserver->onNotifyMouseEvent(event);
            // std::cout << "emit left released" << std::endl;
        }

        if (m_rightButtonPressed) {
            event.eventType = eMouseEventType::MOUSE_RIGHT_BUTTON_PRESSED;
            m_mouseObserver->onNotifyMouseEvent(event);
            // std::cout << "emit right pressed" << std::endl;
        }

        if (m_rightButtonReleased) {
            event.eventType = eMouseEventType::MOUSE_RIGHT_BUTTON_CLICKED;
            m_mouseObserver->onNotifyMouseEvent(event);
            // std::cout << "emit right click" << std::endl;
        }
    }

    // HACK HACK:
    // make -1 to -2, so that we can prevent placeIt/deployIt=false when just stopped viewport dragging
    if (m_mouseMvX2 == -1) {
        m_mouseMvX2 = -2;
    }
    if (m_mouseMvY2 == -1) {
        m_mouseMvY2 = -2;
    }
    m_didMouseMove = false;
    m_leftButtonReleased = false;
    m_rightButtonReleased = false;
    m_mouseScrolledDown = false;
    m_mouseScrolledUp = false;
    m_leftButtonClickedInPreviousFrame = m_leftButtonClicked;
    m_rightButtonClickedInPreviousFrame = m_rightButtonClicked;
}

void cMouse::setCursorPosition(SDL_Window *_windows, int x, int y)
{
    SDL_WarpMouseInWindow(_windows, x, y); // allegro function
    if (m_mouseObserver) {
        s_MouseEvent event {
            eMouseEventType::MOUSE_MOVED_TO,
            cPoint(x,y)};
        m_mouseObserver->onNotifyMouseEvent(event);
    }
}

bool cMouse::isOverRectangle(int x, int y, int width, int height)
{
    return cRectangle::isWithin(getX(), getY(), x, y, width, height);
}

bool cMouse::isOverRectangle(cRectangle *rectangle)
{
    if (rectangle == nullptr) return false;
    return rectangle->isPointWithin(getX(), getY());
}

/**
 * Is player moving map camera with right mouse button pressed/dragging?
 * @return
 */
bool cMouse::isMapScrolling()
{
    return m_mouseMvX1 > -1 && m_mouseMvY1 > -1 && m_mouseMvX2 > -1 && m_mouseMvY2 > -1;
}

bool cMouse::isBoxSelecting()
{
    return m_mouseCoX1 > -1 && m_mouseCoY1 > -1 &&
           m_mouseCoX2 != m_mouseCoX1 && m_mouseCoY2 != m_mouseCoY1 &&
           m_mouseCoX2 > -1 && m_mouseCoY2 > -1;
}

void cMouse::resetDragViewportInteraction()
{
    if (m_mouseMvX1 > -1 || m_mouseMvY1 > -1 || m_mouseMvX2 > -1 || m_mouseMvY2 > -1) {
        m_mouseMvX1 = -1;
        if (m_mouseMvX2 > -1) m_mouseMvX2 = -1;

        m_mouseMvY1 = -1;
        if (m_mouseMvY2 > -1) m_mouseMvY2 = -1;
    }
}

void cMouse::dragViewportInteraction()
{
    if (m_mouseMvX1 < 0 || m_mouseMvY1 < 0) {
        m_mouseMvX1 = m_coords.x;
        m_mouseMvY1 = m_coords.y;
    }
    else {
        // mouse mv 1st coordinates filled
        // when mouse is deviating from this coordinate, draw a line

        if (ABS_length(m_coords.x, m_coords.y, m_mouseMvX1, m_mouseMvY1) > 4) {
            // assign 2nd coordinates
            m_mouseMvX2 = m_coords.x;
            m_mouseMvY2 = m_coords.y;
        }
    }
}

void cMouse::boxSelectLogic(int mouseCell)
{
// When the mouse is pressed, we will check if the first coordinates are filled in
    // if so, we will update the second coordinates. If the player holds his mouse we
    // keep updating the second coordinates and create a rectangle to select units with.
    if (m_mouseCoX1 > -1 && m_mouseCoY1 > -1) {
        if (abs(m_coords.x - m_mouseCoX1) > 4 && abs(m_coords.y - m_mouseCoY1) > 4) {
            // assign 2nd coordinates
            m_mouseCoX2 = m_coords.x;
            if (m_coords.x > game.m_screenW - cSideBar::SidebarWidth) {
                m_mouseCoX2 = game.m_screenW - cSideBar::SidebarWidth - 1;
            }

            m_mouseCoY2 = m_coords.y;
            if (m_mouseCoY2 < cSideBar::TopBarHeight) {
                m_mouseCoY2 = cSideBar::TopBarHeight;
            }

            // and draw the selection box
            m_renderDrawer->renderRectColor(m_mouseCoX1, m_mouseCoY1, m_mouseCoX2-m_mouseCoX1, m_mouseCoY2-m_mouseCoY1, game.getColorFadeSelected(255, 255, 255));
        }

        // Note that we have to fix up the coordinates when checking 'within border'
        // for units (when X2 < X1 for example!)
    }
    else if (mouseCell > -1) {
        // no first coordinates set, so do that here.
        m_mouseCoX1 = m_coords.x;
        m_mouseCoY1 = m_coords.y;
    }
}

cRectangle cMouse::getBoxSelectRectangle()
{
    int min_x, min_y;
    int max_x, max_y;

    // sort out borders:
    if (m_mouseCoX1 < m_mouseCoX2) {
        min_x = m_mouseCoX1;
        max_x = m_mouseCoX2;
    }
    else {
        max_x = m_mouseCoX1;
        min_x = m_mouseCoX2;
    }

    // Y coordinates
    if (m_mouseCoY1 < m_mouseCoY2) {
        min_y = m_mouseCoY1;
        max_y = m_mouseCoY2;
    }
    else {
        max_y = m_mouseCoY1;
        min_y = m_mouseCoY2;
    }
    return cRectangle(min_x, min_y, (max_x-min_x), (max_y-min_y));
}

void cMouse::resetBoxSelect()
{
    m_mouseCoX1 = -1;
    m_mouseCoY1 = -1;
    m_mouseCoX2 = -1;
    m_mouseCoY2 = -1;
}

bool cMouse::isTile(int value)
{
    return m_mouseTile == value;
}

void cMouse::draw()
{
    int mouseDrawX = m_coords.x;
    int mouseDrawY = m_coords.y;

    if (m_mouseTile > -1) {
        // adjust coordinates of drawing according to the specific mouse sprite/tile
        if (m_mouseTile == MOUSE_DOWN) {
            mouseDrawY -= 16;
        }
        else if (m_mouseTile == MOUSE_RIGHT) {
            mouseDrawX -= 16;
        }
        else if (m_mouseTile == MOUSE_MOVE || m_mouseTile == MOUSE_RALLY) {
            mouseDrawX -= 16;
            mouseDrawY -= 16;
        }
        else if (m_mouseTile == MOUSE_ATTACK) {
            mouseDrawX -= 16;
            mouseDrawY -= 16;
        }
        else if (m_mouseTile == MOUSE_REPAIR) {
            mouseDrawX -= 16;
            mouseDrawY -= 16;
        }
        else if (m_mouseTile == MOUSE_FORBIDDEN) {
            mouseDrawX -= 16;
            mouseDrawY -= 16;
        }
        else if (m_mouseTile == MOUSE_PICK) {
            mouseDrawX -= 16;
            mouseDrawY -= 16;
        }
        else {
        }

        if (game.isDebugMode()) {
            if (game.isState(GAME_PLAYING)) {
                int mouseCell = game.getPlayer(HUMAN).getGameControlsContext()->getMouseCell(); // Ugh :/
                m_textDrawer->drawText(0, cSideBar::TopBarHeight + 1, std::format("MouseCell {}", mouseCell));
            }
        }
    }

    auto gfxdata = m_ctx->getGraphicsContext()->gfxdata;
    global_renderDrawer->renderSprite(gfxdata->getTexture(m_mouseTile),mouseDrawX, mouseDrawY);

    if (game.isDebugMode()) {
        int y = mouseDrawY;
        for (auto line: m_debugLines) {
            m_textDrawer->drawText(mouseDrawX + 32, y, line.c_str());
            y += m_textDrawer->getFontHeight() + 2;
        }
    }
}

bool cMouse::isNormalRightClick()
{
    // < -1 means we have had this evaluation before :/
    // poor man's solution as opposed to events
    return m_mouseMvX2 < -1 && m_mouseMvY2 < -1;
}

int cMouse::getMouseDragDeltaX()
{
    return m_mouseMvX2 - m_mouseMvX1;
}

int cMouse::getMouseDragDeltaY()
{
    return m_mouseMvY2 - m_mouseMvY1;
}

cPoint cMouse::getMouseCoords() {
    return cPoint(getX(), getY());
}

cPoint cMouse::getDragLineStartPoint()
{
    return cPoint(m_mouseMvX1, m_mouseMvY1);
}

cPoint cMouse::getDragLineEndPoint()
{
    return cPoint(m_mouseMvX2, m_mouseMvY2);
}

void cMouse::setTile(int value)
{
    if (value != m_mouseTile) {
        logbook(std::format("cMouse::setTile(): Changing mouse tile from {} ({}) to {} ({})", m_mouseTile,
                            mouseTileName(m_mouseTile).c_str(), value, mouseTileName(value).c_str()));
        m_mouseTile = value;
    }
}

std::string cMouse::mouseTileName(int tile)
{
    if (tile == MOUSE_NORMAL) {
        return "MOUSE_NORMAL";
    }
    if (tile == MOUSE_MOVE) {
        return "MOUSE_MOVE";
    }
    if (tile == MOUSE_PICK) {
        return "MOUSE_PICK";
    }
    if (tile == MOUSE_RALLY) {
        return "MOUSE_RALLY";
    }
    if (tile == MOUSE_REPAIR) {
        return "MOUSE_REPAIR";
    }
    return "UNKNOWN";
}

void cMouse::addDebugLine(std::string basicString)
{
    this->m_debugLines.push_back(basicString);
}
