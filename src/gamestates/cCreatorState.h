#pragma once

#include "include/enums.h"
#include "include/eGameState.h"
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

    cGameState *getState(eGameState gameState, bool forceRecreate = false);
private:
    //cGameState *m_states[GameState::MAX];
    EnumArray<std::optional<std::unique_ptr<cGameState>>,eGameState> m_states;
    EnumArray<bool, eGameState> needToRecreateState;
    void createStateFromScratch(eGameState gameState);
    cGame* m_game;
    GameContext* m_ctx;
};
