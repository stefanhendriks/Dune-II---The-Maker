#include "gamestates/cCreatorState.h"

CreatorState::CreatorState()
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
    // no implemented yet
}