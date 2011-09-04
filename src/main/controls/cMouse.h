/*
 * cMouse.h
 *
 *  Created on: 24-okt-2010
 *      Author: Stefan
 */

#ifndef CMOUSE_H_
#define CMOUSE_H_

class cMouse {

	public:
		cMouse();

		static cMouse *getInstance();

		void updateState();

		bool isLeftButtonPressed() { return leftButtonPressed; }
		bool isRightButtonPressed() { return rightButtonPressed;  }

		bool isLeftButtonClicked()  { return leftButtonClicked; }
		bool isRightButtonClicked() { return rightButtonClicked; }

		bool isMouseScrolledUp() { return mouseScrolledUp; }
		bool isMouseScrolledDown() { return mouseScrolledDown;  }

		bool isMouseDraggingRectangle() { return mouseDraggingRectangle; }

		bool isOverRectangle(cRectangle * rect);
		bool isOverRectangleUsingWidthAndHeight(int rectX, int rectY, int width, int height);
		bool isOverRectangleUsingCoordinates(int rectX, int rectY, int rectEndX, int rectEndY);

		int getX() { return x; }
		int getY() { return y; }
		int getZ() { return z; }

		void setMouseTile(int value) { tile = value; }
		int getMouseTile() { return tile; }

		void positionMouseCursor(int x, int y);
		cRectangle *getLastCreatedRectangle() {  return &lastCreatedRectangle;  }
		cRectangle *getCurrentDrawingRectangle() { return &currentRectangle; }

protected:
	~cMouse();


private:
    bool leftButtonPressed;
    bool rightButtonPressed;
    bool leftButtonClicked;
    bool rightButtonClicked;
    bool leftButtonPressedInPreviousFrame;
    bool rightButtonPressedInPreviousFrame;
    bool mouseScrolledUp;
    bool mouseScrolledDown;
    bool mouseDraggingRectangle;
    static cMouse *instance;
    int x, y, z;
    int zValuePreviousFrame;
    int tile;
    cRectangle currentRectangle;
    cRectangle lastCreatedRectangle;
    void doRectangleLogic();
};

#endif /* CMOUSE_H_ */
