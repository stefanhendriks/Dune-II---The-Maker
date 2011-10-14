#ifndef STATEBUILDER_H_
#define STATEBUILDER_H_

#include "State.h"

#include "../player/cPlayer.h"

#include "../include/GameState.h"
#include "../gameobjects/ScreenResolution.h"

#include "../gui/windows/GuiWindowBuilder.h"

class StateBuilder {
	public:
		StateBuilder(ScreenResolution * screenResolution, cPlayer * player);
		~StateBuilder();

		State * buildState(GameState gameState);

	protected:

	private:
		State * buildNewMainMenuState();

		GuiWindowBuilder * guiWindowBuilder;

		ScreenResolution * screenResolution;

		cPlayer * player;
};

#endif
