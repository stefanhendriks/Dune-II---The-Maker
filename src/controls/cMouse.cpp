#include "cMouse.h"

#include "controls/cGameControlsContext.h"
#include "d2tmc.h"
#include "data/gfxdata.h"
#include "drawers/cAllegroDrawer.h"
#include "drawers/cTextDrawer.h"
#include "player/cPlayer.h"
#include "sidebar/cSideBar.h"
#include "utils/d2tm_math.h"

#include <allegro.h>

cMouse::cMouse() : m_textDrawer(bene_font), coords(cPoint(0,0)) {
	z=0;
	leftButtonPressed=false;
	rightButtonPressed=false;
	leftButtonPressedInPreviousFrame=false;
	rightButtonPressedInPreviousFrame=false;
	leftButtonClicked=false;
	rightButtonClicked=false;
	mouseScrolledUp=false;
	mouseScrolledDown=false;
	zValuePreviousFrame = mouse_z;
	_mouseObserver = nullptr; // set later
    debugLines = std::vector<std::string>();

    init();
}

cMouse::~cMouse() {
    _mouseObserver = nullptr; // we do not own this, so don't delete
}

void cMouse::init() {
    mouse_tile = MOUSE_NORMAL;

    mouse_co_x1 = -1;
    mouse_co_y1 = -1;
    mouse_co_x2 = -1;
    mouse_co_y2 = -1;

    mouse_mv_x1 = -1;
    mouse_mv_y1 = -1;
    mouse_mv_x2 = -1;
    mouse_mv_y2 = -1;

    debugLines.clear();
}

void cMouse::updateState() {
    debugLines.clear();

    bool didMouseMove = coords.x != mouse_x || coords.y != mouse_y;
    coords.x = mouse_x;
    coords.y = mouse_y;
    z = mouse_z;

	// check if leftButtonIsPressed=true (which is the previous frame)
    leftButtonPressedInPreviousFrame = leftButtonPressed;
    rightButtonPressedInPreviousFrame = rightButtonPressed;

    leftButtonPressed = mouse_b & 1;
    rightButtonPressed = mouse_b & 2;

	// now check if the leftButtonPressed == false, but the previous frame was true (if so, it is
	// counted as a click)
    leftButtonClicked = (leftButtonPressedInPreviousFrame == true && leftButtonPressed == false);
    rightButtonClicked = (rightButtonPressedInPreviousFrame == true && rightButtonPressed == false);

    mouseScrolledUp = mouseScrolledDown = false;

	if (z > zValuePreviousFrame) {
        mouseScrolledUp = true;
	}

	if (z < zValuePreviousFrame) {
        mouseScrolledDown = true;
	}

    zValuePreviousFrame = z;

	// cap mouse z
	if (z > 10 || z < -10) {
        z = 0;
		position_mouse_z(0); // allegro function
	}

	// mouse moved
	if (_mouseObserver) {
        s_MouseEvent event {
                eMouseEventType::MOUSE_MOVED_TO,
                coords,
                z
        };

        if (didMouseMove) {
            _mouseObserver->onNotifyMouseEvent(event);
        }

        if (mouseScrolledUp) {
            event.eventType = eMouseEventType::MOUSE_SCROLLED_UP;
            _mouseObserver->onNotifyMouseEvent(event);
        }

        if (mouseScrolledDown) {
            event.eventType = eMouseEventType::MOUSE_SCROLLED_DOWN;
            _mouseObserver->onNotifyMouseEvent(event);
        }

        if (leftButtonPressed) {
            event.eventType = eMouseEventType::MOUSE_LEFT_BUTTON_PRESSED;
            _mouseObserver->onNotifyMouseEvent(event);
        }

        if (leftButtonClicked) {
            event.eventType = eMouseEventType::MOUSE_LEFT_BUTTON_CLICKED;
            _mouseObserver->onNotifyMouseEvent(event);
        }

        if (rightButtonPressed) {
            event.eventType = eMouseEventType::MOUSE_RIGHT_BUTTON_PRESSED;
            _mouseObserver->onNotifyMouseEvent(event);
        }

        if (rightButtonClicked) {
            event.eventType = eMouseEventType::MOUSE_RIGHT_BUTTON_CLICKED;
            _mouseObserver->onNotifyMouseEvent(event);
        }
    }

    // HACK HACK:
    // make -1 to -2, so that we can prevent placeIt/deployIt=false when just stopped viewport dragging
    if (mouse_mv_x2 == -1) {
        mouse_mv_x2 = -2;
    }
    if (mouse_mv_y2 == -1) {
        mouse_mv_y2 = -2;
    }
}

void cMouse::positionMouseCursor(int x, int y) {
	position_mouse(x, y); // allegro function
	if (_mouseObserver) {
	    s_MouseEvent event {
                eMouseEventType::MOUSE_MOVED_TO,
                cPoint(0,0),
                0
	    };
        _mouseObserver->onNotifyMouseEvent(event);
    }
}

bool cMouse::isOverRectangle(int x, int y, int width, int height) {
    return cRectangle::isWithin(getX(), getY(), x, y, width, height);
}

bool cMouse::isOverRectangle(cRectangle *rectangle) {
    if (rectangle == nullptr) return false;
    return rectangle->isPointWithin(getX(), getY());
}

/**
 * Is player moving map camera with right mouse button pressed/dragging?
 * @return
 */
bool cMouse::isMapScrolling() {
    return mouse_mv_x1 > -1 && mouse_mv_y1 > -1 && mouse_mv_x2 > -1 && mouse_mv_y2 > -1;
}

bool cMouse::isBoxSelecting() {
    return mouse_co_x1 > -1 && mouse_co_y1 > -1 &&
    mouse_co_x2 != mouse_co_x1 && mouse_co_y2 != mouse_co_y1 &&
    mouse_co_x2 > -1 && mouse_co_y2 > -1;
}

void cMouse::resetDragViewportInteraction() {
    if (mouse_mv_x1 > -1 || mouse_mv_y1 > -1 || mouse_mv_x2 > -1 || mouse_mv_y2 > -1) {
        mouse_mv_x1 = -1;
        if (mouse_mv_x2 > -1) mouse_mv_x2 = -1;

        mouse_mv_y1 = -1;
        if (mouse_mv_y2 > -1) mouse_mv_y2 = -1;
    }
}

void cMouse::dragViewportInteraction() {
    if (mouse_mv_x1 < 0 || mouse_mv_y1 < 0) {
        mouse_mv_x1 = mouse_x;
        mouse_mv_y1 = mouse_y;
    } else {
        // mouse mv 1st coordinates filled
        // when mouse is deviating from this coordinate, draw a line

        if (ABS_length(mouse_x, mouse_y, mouse_mv_x1, mouse_mv_y1) > 4) {
            // assign 2nd coordinates
            mouse_mv_x2 = mouse_x;
            mouse_mv_y2 = mouse_y;
        }
    }
}

void cMouse::boxSelectLogic(int mouseCell) {
// When the mouse is pressed, we will check if the first coordinates are filled in
    // if so, we will update the second coordinates. If the player holds his mouse we
    // keep updating the second coordinates and create a rectangle to select units with.
    if (mouse_co_x1 > -1 && mouse_co_y1 > -1) {
        if (abs(mouse_x - mouse_co_x1) > 4 &&
            abs(mouse_y - mouse_co_y1) > 4) {

            // assign 2nd coordinates
            mouse_co_x2 = mouse_x;
            if (mouse_x > game.m_screenW - cSideBar::SidebarWidth) {
                mouse_co_x2 = game.m_screenW - cSideBar::SidebarWidth - 1;
            }

            mouse_co_y2 = mouse_y;
            if (mouse_co_y2 < cSideBar::TopBarHeight) {
                mouse_co_y2 = cSideBar::TopBarHeight;
            }

            // and draw the selection box
            renderDrawer->drawRectangle(bmp_screen, mouse_co_x1, mouse_co_y1, mouse_co_x2, mouse_co_y2, game.getColorFadeSelected(255, 255, 255));
        }

        // Note that we have to fix up the coordinates when checking 'within border'
        // for units (when X2 < X1 for example!)
    } else if (mouseCell > -1) {
        // no first coordinates set, so do that here.
        mouse_co_x1 = mouse_x;
        mouse_co_y1 = mouse_y;
    }
}

cRectangle cMouse::getBoxSelectRectangle() {
    int min_x, min_y;
    int max_x, max_y;

    // sort out borders:
    if (mouse_co_x1 < mouse_co_x2) {
        min_x = mouse_co_x1;
        max_x = mouse_co_x2;
    } else {
        max_x = mouse_co_x1;
        min_x = mouse_co_x2;
    }

    // Y coordinates
    if (mouse_co_y1 < mouse_co_y2) {
        min_y = mouse_co_y1;
        max_y = mouse_co_y2;
    } else {
        max_y = mouse_co_y1;
        min_y = mouse_co_y2;
    }
    return cRectangle(min_x, min_y, (max_x-min_x), (max_y-min_y));
}

void cMouse::resetBoxSelect() {
    mouse_co_x1 = -1;
    mouse_co_y1 = -1;
    mouse_co_x2 = -1;
    mouse_co_y2 = -1;
}

bool cMouse::isTile(int value) {
    return mouse_tile == value;
}

void cMouse::draw() {
    int mouseDrawX = mouse_x;
    int mouseDrawY = mouse_y;

    if (mouse_tile > -1) {
        // adjust coordinates of drawing according to the specific mouse sprite/tile
        if (mouse_tile == MOUSE_DOWN) {
            mouseDrawY -= 16;
        } else if (mouse_tile == MOUSE_RIGHT) {
            mouseDrawX -= 16;
        } else if (mouse_tile == MOUSE_MOVE || mouse_tile == MOUSE_RALLY) {
            mouseDrawX -= 16;
            mouseDrawY -= 16;
        } else if (mouse_tile == MOUSE_ATTACK) {
            mouseDrawX -= 16;
            mouseDrawY -= 16;
        } else if (mouse_tile == MOUSE_REPAIR) {
            mouseDrawX -= 16;
            mouseDrawY -= 16;
        } else if (mouse_tile == MOUSE_FORBIDDEN) {
            mouseDrawX -= 16;
            mouseDrawY -= 16;
        } else if (mouse_tile == MOUSE_PICK) {
            mouseDrawX -= 16;
            mouseDrawY -= 16;
        } else {
        }

        if (game.isDebugMode()) {
            if (game.isState(GAME_PLAYING)) {
                int mouseCell = players[HUMAN].getGameControlsContext()->getMouseCell(); // Ugh :/
                m_textDrawer.drawTextWithOneInteger(0, cSideBar::TopBarHeight + 1, "MouseCell %d", mouseCell);
            }
        }
    }

    renderDrawer->drawSprite(bmp_screen, mouse_tile, mouseDrawX, mouseDrawY);

    if (game.isDebugMode()) {
        int y = mouseDrawY;
        for (auto line: debugLines) {
            m_textDrawer.drawText(mouseDrawX + 32, y, line.c_str());
            y += m_textDrawer.getFontHeight() + 2;
        }
    }
}

bool cMouse::isNormalRightClick() {
    // < -1 means we have had this evaluation before :/
    // poor man's solution as opposed to events
    return mouse_mv_x2 < -1 && mouse_mv_y2 < -1;
}

int cMouse::getMouseDragDeltaX() {
    return mouse_mv_x2 - mouse_mv_x1;
}

int cMouse::getMouseDragDeltaY() {
    return mouse_mv_y2 - mouse_mv_y1;
}

cPoint cMouse::getDragLineStartPoint() {
    return cPoint(mouse_mv_x1, mouse_mv_y1);
}

cPoint cMouse::getDragLineEndPoint() {
    return cPoint(mouse_mv_x2, mouse_mv_y2);
}

void cMouse::setTile(int value) {
    if (value != mouse_tile) {
        logbook(fmt::format("cMouse::setTile(): Changing mouse tile from {} ({}) to {} ({})", mouse_tile,
            mouseTileName(mouse_tile).c_str(), value, mouseTileName(value).c_str()));
        mouse_tile = value;
    }
}

std::string cMouse::mouseTileName(int tile) {
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

void cMouse::addDebugLine(std::string basicString) {
    this->debugLines.push_back(basicString);
}
