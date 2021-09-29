#include "../include/d2tmh.h"

cMouse::cMouse() {
	x=y=z=0;
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
}

cMouse::~cMouse() {
    _mouseObserver = nullptr; // we do not own this, so don't delete
}

void cMouse::updateState() {
    bool didMouseMove = x != mouse_x || y != mouse_y;
    x = mouse_x;
    y = mouse_y;
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
                x,
                y,
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
                0,
                0,
                0
	    };
        _mouseObserver->onNotifyMouseEvent(event);
    }
}

bool cMouse::isOverRectangle(int x, int y, int width, int height) {
    return cRectangle::isWithin(getX(), getY(), x, y, width, height);
}

bool cMouse::isOverRectangle(cRectangle *rectangle) {
    return rectangle->isMouseOver(getX(), getY());
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
