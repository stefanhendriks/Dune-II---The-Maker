/*
 * cGameObject.h
 *
 *  Created on: 7-nov-2010
 *      Author: Stefan
 */

// a game object is the most super class of all game objects
// it has the bare bones that all other objects (structures, units, bullets, etc) use.

#ifndef CGAMEOBJECT_H_
#define CGAMEOBJECT_H_

class cGameObject {

	public:
		cGameObject();
		~cGameObject();

		void setX(int value) { x = value; }
		void setY(int value) { y = value; }

		int getX() { return x; }
		int getY() { return y; }

	protected:

	private:
		int x;
		int y;

		int frame; // the frame to draw
};

#endif /* CGAMEOBJECT_H_ */
