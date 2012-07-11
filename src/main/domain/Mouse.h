#ifndef CMOUSE_H_
#define CMOUSE_H_

#include <allegro.h>
#include "../gameobjects/Rectangle.h"

class Mouse {

	public:
		Mouse();

		static Mouse *getInstance();

		void updateState();

		bool isLeftButtonPressed() { return leftButtonPressed; }
		bool isRightButtonPressed() { return rightButtonPressed;  }

		bool isLeftButtonClicked()  { return leftButtonClicked; }
		bool isRightButtonClicked() { return rightButtonClicked; }

		bool isMouseScrolledUp() { return mouseScrolledUp; }
		bool isMouseScrolledDown() { return mouseScrolledDown;  }

		bool isMouseDraggingRectangle() { return mouseDraggingRectangle; }

		bool isOverRectangle(Rectangle * rect);
		bool isOverRectangleUsingWidthAndHeight(int rectX, int rectY, int width, int height);
		bool isOverRectangleUsingCoordinates(int rectX, int rectY, int rectEndX, int rectEndY);

		int getX() { return x; }
		int getY() { return y; }
		int getZ() { return z; }

		void setMouseTile(int value) { tile = value; }
		int getMouseTile() { return tile; }

		void positionMouseCursor(int x, int y);
		Rectangle *getLastCreatedRectangle() {  return &lastCreatedRectangle;  }
		Rectangle *getCurrentDrawingRectangle() { return &currentRectangle; }

		void draw(BITMAP * screen);

protected:
	~Mouse();


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
    static Mouse *instance;
    int x, y, z;
    int zValuePreviousFrame;
    int tile;
    Rectangle currentRectangle;
    Rectangle lastCreatedRectangle;
    void doRectangleLogic();
};

#endif /* CMOUSE_H_ */
