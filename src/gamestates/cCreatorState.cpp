#include "gamestates/cCreatorState.h"
#include "utils/cLog.h"
#include "gamestates/cGameState.h"

#include "gamestates/cMainMenuState.h"
#include "gamestates/cCreditsState.h"
#include "gamestates/cWinLoseState.h"
#include "gamestates/cMentatState.h"
#include "gamestates/cTellHouseState.h"
#include "gamestates/cChooseHouseState.h"
#include "gamestates/cSelectYourNextConquestState.h"

#include "cGame.h"
#include "context/GameContext.hpp"

CreatorState::CreatorState(cGame* game, GameContext* ctx): m_game(game), m_ctx(ctx)
{
    // all State should be recreate when needed to use
    needToRecreateState.fill(true);
    // this States should not be recreated when we need to use
    needToRecreateState[GameState::OPTIONS] = false;
    needToRecreateState[GameState::SETUPSKIRMISH] = false;
    needToRecreateState[GameState::CREDITS] = false;
    needToRecreateState[GameState::MISSIONSELECT] = false;
    needToRecreateState[GameState::MENU] = false;
    needToRecreateState[GameState::WINNING] = false;
    needToRecreateState[GameState::LOSING] = false;
}

CreatorState::~CreatorState()
{

}


cGameState* CreatorState::getState(GameState gameState, bool forceRecreate)
{
    // no existing state ...
    if (!m_states[gameState].has_value()) {
        createStateFromScratch(gameState);
        return m_states[gameState].value().get();
    }

    // state already exist
    if (needToRecreateState[gameState] || forceRecreate) {
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
        m_states[GameState::MENU] = std::make_unique<cMainMenuState>(*m_game, m_ctx);
        break;

    case GameState::CREDITS:
        m_states[GameState::CREDITS] = std::make_unique<cCreditsState>(*m_game, m_ctx);
        break;

    case GameState::WINNING:
        m_states[GameState::WINNING] = std::make_unique<cWinLoseState>(*m_game, m_ctx, Outcome::Lose);
        break;

    case GameState::LOSING:
        m_states[GameState::LOSING] = std::make_unique<cWinLoseState>(*m_game, m_ctx, Outcome::Lose);
        break;

    case GameState::BRIEFING:
        m_states[GameState::BRIEFING] = std::make_unique<cMentatState>(*m_game, m_ctx, MentatMode::Briefing, m_game->getDataCampaign());
        break;

    case GameState::WINBRIEF:
        m_states[GameState::WINBRIEF] = std::make_unique<cMentatState>(*m_game, m_ctx, MentatMode::WinBrief, m_game->getDataCampaign());
        break;

    case GameState::LOSEBRIEF:
        m_states[GameState::LOSEBRIEF] = std::make_unique<cMentatState>(*m_game, m_ctx, MentatMode::LoseBrief, m_game->getDataCampaign());
        break;

    case GameState::TELLHOUSE:
        m_states[GameState::TELLHOUSE] = std::make_unique<cTellHouseState>(*m_game, m_ctx, m_game->getDataCampaign());
        break;

    case GameState::SELECT_HOUSE:
        m_states[GameState::SELECT_HOUSE] = std::make_unique<cChooseHouseState>(*m_game, m_ctx);
        break;

    case GameState::REGION:
        m_states[GameState::REGION] = std::make_unique<cSelectYourNextConquestState>(*m_game, m_ctx, m_game->getDataCampaign());
        break;

    // @mira : i prefer to rip default mode and have the compiler tell me what I've forgotten
    default:
        cLogger::getInstance()->log(LOG_ERROR, COMP_SETUP, "CreatorState",
                                     std::format("Forget gameState {}",gameStateToString(gameState)));
        break;
    }
}