#pragma once
#ifndef PLAYING_STATE_H
#define PLAYING_STATE_H

#include <assert.h>
#include <cstddef>

#include "State.h"

#include "../domain/Map.h"
#include "../domain/Viewport.h"

#include "../utils/Logger.h"

class PlayingState : public State {

	public:
		PlayingState(Screen * screen, Mouse * mouse, Map * map, Viewport * viewPort) : State(screen, mouse) {
			if (map == NULL) throw NullArgumentException;
			if (viewPort == NULL) throw NullArgumentException;
			this->map = map;
			this->viewPort = viewPort;
		}

		~PlayingState() {
		}

		void setViewportDrawLocation(int x, int y) {
			viewPortDrawLocation.set(x, y);
		}

		void draw();
		void update();

	protected:

	private:
		Map * map;
		Viewport * viewPort;
		Vector2D viewPortDrawLocation;
		Vector2D viewPortCamera;

};



#endif