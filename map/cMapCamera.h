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

		void jumpTo(int	theX, int theY);
		void moveTo(int theX, int theY);

		int getX() { return x; }
		int getY() { return y; }
		int getTargetX() { return targetX; }
		int getTargetY() { return targetY; }

		int getEndX() { return x + viewportWidth; }
		int getEndY() { return y + viewportHeight; }

		void thinkInteraction();

	protected:


	private:
		// the width and height of the viewport
		// calculated at the constructor
		int viewportWidth;
		int viewportHeight;

		// the X,Y position (top left corner of the viewport)
		// the targetX and targetY are leading.
		int x, targetX;
		int y, targetY;

		// timer used, when
		int TIMER_move;
};

#endif /* CMAPCAMERA_H_ */
