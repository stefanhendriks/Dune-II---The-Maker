#pragma once

#include "include/eGameState.h"
#include "gamestates/cGameState.h"
#include "utils/cEnumArray.h"
#include "include/sDataCampaign.h"

#include <memory>
#include <optional>

class GameContext;
class cGameInterface;
class cIni;

class cCreatorState {
public:
    explicit cCreatorState(sGameServices* services, cIni* ini, s_DataCampaign* dataCampaign);
    ~cCreatorState();

    cGameState *getState(eGameState gameState, bool forceRecreate = false);
    [[nodiscard]] bool hasState(eGameState gameState) const;
    void destroyState(eGameState gameState);
    void destroyAllStates();

private:
    EnumArray<std::optional<std::unique_ptr<cGameState>>,eGameState> m_states;
    EnumArray<bool, eGameState> needToRecreateState;
    sGameServices* m_services = nullptr;
    s_DataCampaign* m_dataCampaign = nullptr;
    cGameInterface* m_interface = nullptr;
    cIni* m_cIni = nullptr;

    void createStateFromScratch(eGameState gameState);
};
