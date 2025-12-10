#pragma once

#include "include/enums.h"
#include "enums GameState.h"
#include "gamestates/cGameState.h"
#include "utils/cEnumArray.h"

#include <memory>
#include <optional>

class cGame;
class GameContext;

class CreatorState {
public:
    explicit CreatorState(cGame* game, GameContext* ctx);
    ~CreatorState();

    cGameState *getState(GameState gameState, bool forceRecreate = false);
private:
    //cGameState *m_states[GameState::MAX];
    EnumArray<std::optional<std::unique_ptr<cGameState>>,GameState> m_states;
    EnumArray<bool, GameState> needToRecreateState;
    void createStateFromScratch(GameState gameState);
    cGame* m_game;
    GameContext* m_ctx;
};
