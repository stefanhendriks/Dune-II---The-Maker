#include "gamestates/cCreatorState.h"
#include "utils/cLog.h"
#include "gamestates/cGameState.h"

#include "gamestates/cMainMenuState.h"
#include "gamestates/cEditorState.h"
#include "gamestates/cNewMapEditorState.h"
#include "gamestates/cCreditsState.h"
#include "gamestates/cWinLoseState.h"
#include "gamestates/cMentatState.h"
#include "gamestates/cTellHouseState.h"
#include "gamestates/cChooseHouseState.h"
#include "gamestates/cGamePlaying.h"
#include "gamestates/cSelectYourNextConquestState.h"
#include "gamestates/cOptionsState.h"

#include "game/cGame.h"
#include "context/GameContext.hpp"
#include <cassert>

CreatorState::CreatorState(cGame* game, sGameServices* services): m_game(game), m_services(services)
{
    assert(game != nullptr);
    assert(m_services != nullptr);
    // all State should be recreate when needed to use
    needToRecreateState.fill(true);
    // this States should not be recreated when we need to use
    needToRecreateState[eGameState::OPTIONS] = false;
    needToRecreateState[eGameState::PLAYING] = false;
    needToRecreateState[eGameState::SETUPSKIRMISH] = false;
    needToRecreateState[eGameState::CREDITS] = false;
    needToRecreateState[eGameState::MISSIONSELECT] = false;
    needToRecreateState[eGameState::MENU] = false;
    needToRecreateState[eGameState::EDITOR] = false;
    needToRecreateState[eGameState::NEW_MAP_EDITOR] = false;
    needToRecreateState[eGameState::WINNING] = false;
    needToRecreateState[eGameState::LOSING] = false;
}

CreatorState::~CreatorState()
{

}


cGameState* CreatorState::getState(eGameState gameState, bool forceRecreate)
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

void CreatorState::createStateFromScratch(eGameState gameState)
{
    switch (gameState)
    {
    case eGameState::MENU:
        m_states[eGameState::MENU] = std::make_unique<cMainMenuState>(*m_game, m_services);
        break;

    case eGameState::CREDITS:
        m_states[eGameState::CREDITS] = std::make_unique<cCreditsState>(*m_game, m_services);
        break;

    case eGameState::EDITOR:
        m_states[eGameState::EDITOR] = std::make_unique<cEditorState>(*m_game, m_services);
        break;

    case eGameState::NEW_MAP_EDITOR:
        m_states[eGameState::EDITOR] = std::make_unique<cNewMapEditorState>(*m_game, m_services);
        break;

    case eGameState::WINNING:
        m_states[eGameState::WINNING] = std::make_unique<cWinLoseState>(*m_game, m_services, Outcome::Lose);
        break;

    case eGameState::LOSING:
        m_states[eGameState::LOSING] = std::make_unique<cWinLoseState>(*m_game, m_services, Outcome::Lose);
        break;

    case eGameState::BRIEFING:
        m_states[eGameState::BRIEFING] = std::make_unique<cMentatState>(*m_game, m_services, MentatMode::Briefing, m_game->getDataCampaign());
        break;

    case eGameState::WINBRIEF:
        m_states[eGameState::WINBRIEF] = std::make_unique<cMentatState>(*m_game, m_services, MentatMode::WinBrief, m_game->getDataCampaign());
        break;

    case eGameState::LOSEBRIEF:
        m_states[eGameState::LOSEBRIEF] = std::make_unique<cMentatState>(*m_game, m_services, MentatMode::LoseBrief, m_game->getDataCampaign());
        break;

    case eGameState::TELLHOUSE:
        m_states[eGameState::TELLHOUSE] = std::make_unique<cTellHouseState>(*m_game, m_services, m_game->getDataCampaign());
        break;

    case eGameState::SELECT_HOUSE:
        m_states[eGameState::SELECT_HOUSE] = std::make_unique<cChooseHouseState>(*m_game, m_services);
        break;

    case eGameState::REGION:
        m_states[eGameState::REGION] = std::make_unique<cSelectYourNextConquestState>(*m_game, m_services, m_game->getDataCampaign());
        break;

    case eGameState::PLAYING:
        m_states[eGameState::PLAYING] = std::make_unique<cGamePlaying>(*m_game, m_services);
        break;

    case eGameState::OPTIONS:
        m_states[eGameState::OPTIONS] = std::make_unique<cOptionsState>(*m_game, m_services, m_game->getCurrentState());
        break;

    // @mira : i prefer to rip default mode and have the compiler tell me what I've forgotten
    default:
        cLogger::getInstance()->log(LOG_ERROR, COMP_SETUP, "CreatorState",
                                     std::format("Forget gameState {}",gameStateToString(gameState)));
        break;
    }
}