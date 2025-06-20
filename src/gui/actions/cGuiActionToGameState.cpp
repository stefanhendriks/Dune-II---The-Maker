#include "cGuiActionToGameState.h"

#include "d2tmc.h"

cGuiActionToGameState::cGuiActionToGameState(int nextState, bool fadeOut) : fadeOut(fadeOut), nextState(nextState)
{
}

void cGuiActionToGameState::execute()
{
    game.setNextStateToTransitionTo(nextState);
    if (fadeOut) {
        game.initiateFadingOut();
    }
}