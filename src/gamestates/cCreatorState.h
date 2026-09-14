/**
 * @file cCreatorState.h
 *
 * Dune 2 - The Maker
 *
 * @author Stefan Hendriks & the D2TM Team
 * @www http://www.dune2themaker.com
 * @copyright Copyright (c) 2001 - 2026 D2TM Team
 * @license This software is released under the MIT License.
 *
 * Note: Dune 2 is a trademark of Westwood Studios/Electronic Arts.
 *
 * This is a non-commercial educational project.
 */

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
    explicit cCreatorState(sGameServices *services, cIni *ini, s_DataCampaign *dataCampaign);

    ~cCreatorState();

    cGameState *getState(eGameState gameState);

    cGameState *getOrCreateState(eGameState gameState, bool forceRecreate = false);

    [[nodiscard]] bool hasState(eGameState gameState) const;

    void destroyState(eGameState gameState);

    void destroyAllStates();

private:
    EnumArray<std::optional<std::unique_ptr<cGameState> >, eGameState> m_states;
    sGameServices *m_services = nullptr;
    s_DataCampaign *m_dataCampaign = nullptr;
    cGameInterface *m_interface = nullptr;
    cIni *m_cIni = nullptr;

    void createStateFromScratch(eGameState gameState);
};
