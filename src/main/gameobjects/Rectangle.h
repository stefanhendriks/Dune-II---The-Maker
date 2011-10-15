#ifndef CRECTANGLE_H_
#define CRECTANGLE_H_

#include <assert.h>

class Rectangle {
	public:

		Rectangle();
		Rectangle(int startX, int startY, int endX, int endY);
		Rectangle(Rectangle * other);

		~Rectangle();

		void setStartX(int value) { startX = value; }
		void setStartY(int value) { startY = value; }
		void setEndX(int value) { endX = value; }
		void setEndY(int value) { endY = value; }

		int getStartX() { return startX; }
		int getStartY() { return startY; }
		int getEndX() { return endX; }
		int getEndY() { return endY; }

		int getLowestX() { return startX < endX ? startX : endX; }
		int getLowestY() { return startY < endY ? startY : endY; }

		int getHighestX() { return startX < endX ? endX : startX; }
		int getHighestY() { return startY < endY ? endY : startY; }

		int getHeight() { return getHighestY() - getLowestY(); }
		int getWidth() { return getHighestX() - getLowestX(); }

		bool hasValidCoordinates() { return startX > -1 && startY > -1 && endX > -1 && endY > -1; }
		void resetCoordinates() { startX = startY = endX = endY = -1; }
		void setCoordinates(Rectangle * otherRectangle);

		bool isCoordinateWithinRectangle(int x, int y);

	protected:

	private:
		int startX, startY;
		int endX, endY;
};

#endif /* CRECTANGLE_H_ */
