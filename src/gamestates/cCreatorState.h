#pragma once

#include "include/enums.h"
#include "gamestates/cGameState.h"
#include "utils/cEnumArray.h"

class CreatorState {
public:
    explicit CreatorState();
    ~CreatorState();

    cGameState *getState(GameState gameState) const;
private:
    //cGameState *m_states[GameState::MAX];
    EnumArray<cGameState *,GameState> m_states;
    EnumArray<bool, GameState> needToRecreateState;
};
