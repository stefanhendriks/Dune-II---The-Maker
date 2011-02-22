/*
 * cMapCamera.h
 *
 *  Created on: 10-aug-2010
 *      Author: Stefan
 *
 *  A map camera is 'looking' at the map. Using this camera (or viewport) we can determine
 *  what to draw, how big, etc.
 *
 *  Using a camera technique we can easily adjust the drawing logic whenever the resolution
 *  of the game changes.
 *
 *  The player can move the camera around, the thinkInteraction() method handles the keyboard
 *  and mouse logic (for now: TODO: abstract this behavior in own classes?)
 *
 */

#ifndef CMAPCAMERA_H_
#define CMAPCAMERA_H_

#define TILESIZE_WIDTH_PIXELS 32
#define TILESIZE_HEIGHT_PIXELS 32

class cMapCamera {
	public:
		cMapCamera();
		~cMapCamera();

		void think();

		void jumpTo(int theX, int theY);
		void moveTo(int theX, int theY);

		int getX() {
			return x;
		}
		int getY() {
			return y;
		}
		int getTargetX() {
			return targetX;
		}
		int getTargetY() {
			return targetY;
		}

		int getEndX() {
			return x + viewportWidth;
		}
		int getEndY() {
			return y + viewportHeight;
		}

		int getViewportWidth() {
			return viewportWidth;
		}
		int getViewportHeight() {
			return viewportHeight;
		}

		void thinkInteraction();

		void centerAndJumpViewPortToCell(int cell);

	protected:

	private:
		int viewportWidth;
		int viewportHeight;
		int x, targetX;
		int y, targetY;
		int TIMER_move;

		bool canScrollRight();
		bool canScrollLeft();
		bool canScrollUp();
		bool canScrollDown();

		bool isMouseSelectingBoxUsed();

		bool shouldScrollLeft();
		bool shouldScrollRight();
		bool shouldScrollDown();
		bool shouldScrollUp();
};

#endif /* CMAPCAMERA_H_ */
