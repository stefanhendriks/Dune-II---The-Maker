#include "gamestates/cCreatorState.h"
#include "utils/Log.h"
#include "utils/ini.h"
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
#include "gamestates/cSelectMissionState.h"
#include "gamestates/cSetupSkirmishState.h"
#include "game/cGameInterface.h"

#include "context/GameContext.hpp"
#include "context/cGameObjectContext.h"
#include "include/cAssert.h"

CreatorState::CreatorState(sGameServices* services, cIni* ini, s_DataCampaign* dataCampaign)
    : m_services(services)
{
    m_dataCampaign = dataCampaign;
    d2tm_assert(m_services != nullptr);
    m_cIni = ini;
    d2tm_assert(m_cIni != nullptr);
    d2tm_assert(m_dataCampaign != nullptr);
    m_interface = m_services->ctx->getGameInterface();
    d2tm_assert(m_interface != nullptr);
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

bool CreatorState::hasState(eGameState gameState) const
{
    return m_states[gameState].has_value();
}

void CreatorState::destroyState(eGameState gameState)
{
    m_states[gameState].reset();
}

void CreatorState::destroyAllStates()
{
    for (int i = 0; i < static_cast<int>(eGameState::COUNT); ++i) {
        destroyState(static_cast<eGameState>(i));
    }
}

void CreatorState::createStateFromScratch(eGameState gameState)
{
    switch (gameState)
    {
    case eGameState::MENU:
        m_states[eGameState::MENU] = std::make_unique<cMainMenuState>(m_services);
        break;

    case eGameState::CREDITS:
        m_states[eGameState::CREDITS] = std::make_unique<cCreditsState>(m_services);
        break;

    case eGameState::EDITOR:
        m_states[eGameState::EDITOR] = std::make_unique<cEditorState>(m_services);
        break;

    case eGameState::NEW_MAP_EDITOR:
        m_states[eGameState::NEW_MAP_EDITOR] = std::make_unique<cNewMapEditorState>(m_services);
        break;

    case eGameState::WINNING:
        m_states[eGameState::WINNING] = std::make_unique<cWinLoseState>(m_services, Outcome::Win);
        break;

    case eGameState::LOSING:
        m_states[eGameState::LOSING] = std::make_unique<cWinLoseState>(m_services, Outcome::Lose);
        break;

    case eGameState::BRIEFING:
        m_states[eGameState::BRIEFING] = std::make_unique<cMentatState>(m_services, MentatMode::Briefing, m_cIni, m_dataCampaign);
        break;

    case eGameState::WINBRIEF:
        m_states[eGameState::WINBRIEF] = std::make_unique<cMentatState>(m_services, MentatMode::WinBrief, m_cIni, m_dataCampaign);
        break;

    case eGameState::LOSEBRIEF:
        m_states[eGameState::LOSEBRIEF] = std::make_unique<cMentatState>(m_services, MentatMode::LoseBrief, m_cIni, m_dataCampaign);
        break;

    case eGameState::TELLHOUSE:
        m_states[eGameState::TELLHOUSE] = std::make_unique<cTellHouseState>(m_services, m_cIni, m_dataCampaign);
        break;

    case eGameState::SELECT_HOUSE:
        m_states[eGameState::SELECT_HOUSE] = std::make_unique<cChooseHouseState>(m_services);
        break;

    case eGameState::REGION:
        m_states[eGameState::REGION] = std::make_unique<cSelectYourNextConquestState>(m_services, m_cIni, m_dataCampaign);
        break;

    case eGameState::PLAYING:
        m_states[eGameState::PLAYING] = std::make_unique<cGamePlaying>(m_services);
        break;

    case eGameState::OPTIONS:
        m_states[eGameState::OPTIONS] = std::make_unique<cOptionsState>(m_services, m_interface->getCurrentState());
        break;

    case eGameState::MISSIONSELECT:
        m_states[eGameState::MISSIONSELECT] = std::make_unique<cSelectMissionState>(m_services, m_interface->getCurrentState());
        break;

    case eGameState::SETUPSKIRMISH:
        m_states[eGameState::SETUPSKIRMISH] = std::make_unique<cSetupSkirmishState>(
            m_services,
            m_services->objects->getPreviewMaps(),
            m_dataCampaign);
        break;

    // @mira : i prefer to rip default mode and have the compiler tell me what I've forgotten
    default:
        Logger::fatal(COMP_SETUP, "CreatorState", "Forget gameState {}",gameStateToString(gameState));
        break;
    }
}