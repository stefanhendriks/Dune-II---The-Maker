/**
 * @file ScenarioFlow.hpp
 *
 * Dune 2 - The Maker
 *
 * @author Stefan Hendriks & the D2TM Team
 * @www http://www.dune2themaker.com
 * @copyright Copyright (c) 2002 - 2026 Stefan Hendriks and contributors
 * @license This software is released under the MIT License. See LICENSE.md.
 *
 * Note: Dune 2 is a trademark of Westwood Studios/Electronic Arts.
 *
 * This is a non-commercial educational project.
 */

#pragma once

#include <functional>
#include <utility>

class cPlayer;

/**
 * Narrow entry point to setting up a scenario (players, map, controlled player).
 * Holds callbacks so it does not depend on cGame.
 */
class ScenarioFlow {
public:
    using Callback = std::function<void()>;
    using IntCallback = std::function<void(int)>;
    using PlayerCallback = std::function<void(cPlayer*)>;

    ScenarioFlow(Callback setupPlayers, Callback loadScenario, IntCallback loadMapFromEditor, PlayerCallback setPlayerToInteractFor)
        : m_setupPlayers(std::move(setupPlayers)),
          m_loadScenario(std::move(loadScenario)),
          m_loadMapFromEditor(std::move(loadMapFromEditor)),
          m_setPlayerToInteractFor(std::move(setPlayerToInteractFor)) {
    }

    void setupPlayers() const {
        m_setupPlayers();
    }

    void loadScenario() const {
        m_loadScenario();
    }

    void loadMapFromEditor(int iSkirmishMap) const {
        m_loadMapFromEditor(iSkirmishMap);
    }

    void setPlayerToInteractFor(cPlayer* player) const {
        m_setPlayerToInteractFor(player);
    }

private:
    Callback m_setupPlayers;
    Callback m_loadScenario;
    IntCallback m_loadMapFromEditor;
    PlayerCallback m_setPlayerToInteractFor;
};
