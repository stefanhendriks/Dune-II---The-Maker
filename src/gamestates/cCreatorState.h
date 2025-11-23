#pragma once

#include "include/enums.h"
#include "gamestates/cGameState.h"
#include "utils/cEnumArray.h"

#include <memory>
#include <optional>

class cGame;
class Context;

class CreatorState {
public:
    explicit CreatorState(cGame* game, Context* ctx);
    ~CreatorState();

    cGameState *getState(GameState gameState);
private:
    //cGameState *m_states[GameState::MAX];
    EnumArray<std::optional<std::unique_ptr<cGameState>>,GameState> m_states;
    EnumArray<bool, GameState> needToRecreateState;
    void createStateFromScratch(GameState gameState);
    cGame* m_game;
    Context* m_ctx;
};
