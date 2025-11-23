#include "gamestates/cCreatorState.h"

CreatorState::CreatorState()
{
    m_states.fill(nullptr);
    //all State should be recreate when needed to use
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


cGameState CreatorState::*getState(GameState gameState)
{
    return nullptr;
}