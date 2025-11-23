#include "gamestates/cCreatorState.h"
#include "utils/cLog.h"
#include "gamestates/cGameState.h"

#include "gamestates/cMainMenuState.h"

#include "cGame.h"
#include "context/GameContext.hpp"

CreatorState::CreatorState(cGame* game, Context* ctx): m_game(game), m_ctx(ctx)
{
    // all State should be recreate when needed to use
    needToRecreateState.fill(true);
    // this States should not be recreated when we need to use
    needToRecreateState[GameState::OPTIONS] = false;
    needToRecreateState[GameState::SETUPSKIRMISH] = false;
    needToRecreateState[GameState::CREDITS] = false;
    needToRecreateState[GameState::MISSIONSELECT] = false;
    needToRecreateState[GameState::MENU] = false;
}

CreatorState::~CreatorState()
{

}


cGameState* CreatorState::getState(GameState gameState)
{
    // no existing state ...
    if (!m_states[gameState].has_value()) {
        createStateFromScratch(gameState);
        return m_states[gameState].value().get();
    }

    // state already exist
    if (needToRecreateState[gameState]) {
        createStateFromScratch(gameState);
        return m_states[gameState].value().get();
    } else {
        return m_states[gameState].value().get();
    }
}

void CreatorState::createStateFromScratch(GameState gameState)
{
    switch (gameState)
    {
    case GameState::MENU:
        m_states[GameState::MENU] = std::make_unique<cMainMenuState>(m_game, m_ctx);
        break;
    
    default:
        cLogger::getInstance()->log(LOG_ERROR, COMP_SETUP, "CreatorState",
                                     std::format("Forget gameState {}",gameStateToString(gameState)));
        break;
    }
}