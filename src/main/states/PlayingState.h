#pragma once
#ifndef PLAYING_STATE_H
#define PLAYING_STATE_H

#include <assert.h>
#include <cstddef>

#include "State.h"

#include "../domain/Map.h"

#include "../utils/Logger.h"

class PlayingState : public State {

	public:
		PlayingState(Screen * screen, Mouse * mouse, Map * map) : State(screen, mouse) {
			if (map == NULL) {
				throw NullArgumentException;
			}
			this->map = map; 
		}

		~PlayingState() {
		}

		void draw();
		void update();

	protected:

	private:
		Map * map;

};



#endif