#pragma once

#include "include/enums.h"
#include "gamestates/cGameState.h"
#include "utils/cEnumArray.h"

#include <memory>
#include <optional>

class CreatorState {
public:
    explicit CreatorState();
    ~CreatorState();

    cGameState *getState(GameState gameState);
private:
    //cGameState *m_states[GameState::MAX];
    EnumArray<std::optional<std::unique_ptr<cGameState>>,GameState> m_states;
    EnumArray<bool, GameState> needToRecreateState;
    void createStateFromScratch(GameState gameState);
};
