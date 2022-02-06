#pragma once

#include "utils/cPoint.h"

// a game object is the most super class of all game objects
// it has the bare bones that all other objects (structures, units, bullets, etc) use.
class cGameObject {
	public:
		cGameObject();

		void setLocation(cPoint location) { m_location = location; }

		cPoint getLocation() const { return m_location; }

	private:
        cPoint m_location;
		int m_frame; // the frame to draw
};
