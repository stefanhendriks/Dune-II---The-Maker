#pragma once

#include "include/enums.h"
#include "include/eGameState.h"
#include "gamestates/cGameState.h"
#include "utils/cEnumArray.h"
#include "include/sDataCampaign.h"

#include <memory>
#include <optional>

class GameContext;
class cGameInterface;

class CreatorState {
public:
    explicit CreatorState(sGameServices* services, s_DataCampaign* dataCampaign);
    ~CreatorState();

    cGameState *getState(eGameState gameState, bool forceRecreate = false);
private:
    //cGameState *m_states[GameState::MAX];
    EnumArray<std::optional<std::unique_ptr<cGameState>>,eGameState> m_states;
    EnumArray<bool, eGameState> needToRecreateState;
    void createStateFromScratch(eGameState gameState);
    sGameServices* m_services = nullptr;
    s_DataCampaign* m_dataCampaign = nullptr;
    cGameInterface* m_interface = nullptr;
};
