#pragma once
#ifndef PLAYING_STATE_H
#define PLAYING_STATE_H

#include <assert.h>
#include <cstddef>

#include "State.h"

#include "../utils/Logger.h"

class PlayingState : public State {

	public:
		PlayingState(Screen * screen, Mouse * mouse) : State(screen, mouse) {
			// 
		}

		~PlayingState() {
		}

		void draw();
		void update();

	protected:

	private:


};



#endif