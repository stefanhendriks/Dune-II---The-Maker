/**
 * @file MissionFlow.hpp
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

/**
 * Narrow entry point to the mission related flow of the game (initializing, winning, losing, jumping to a mission).
 * Holds callbacks so it does not depend on cGame.
 */
class MissionFlow {
public:
    using Callback = std::function<void()>;
    using IntCallback = std::function<void(int)>;

    MissionFlow(Callback init, Callback setWon, Callback setLost, Callback checkWinOrFail,
                IntCallback jumpToSelectYourNextConquest, IntCallback goingToWinLoseBrief)
        : m_init(std::move(init)),
          m_setWon(std::move(setWon)),
          m_setLost(std::move(setLost)),
          m_checkWinOrFail(std::move(checkWinOrFail)),
          m_jumpToSelectYourNextConquest(std::move(jumpToSelectYourNextConquest)),
          m_goingToWinLoseBrief(std::move(goingToWinLoseBrief)) {
    }

    void init() const {
        m_init();
    }

    void setWon() const {
        m_setWon();
    }

    void setLost() const {
        m_setLost();
    }

    void checkWinOrFail() const {
        m_checkWinOrFail();
    }

    void jumpToSelectYourNextConquest(int mission) const {
        m_jumpToSelectYourNextConquest(mission);
    }

    void goingToWinLoseBrief(int winOrLoseBrief) const {
        m_goingToWinLoseBrief(winOrLoseBrief);
    }

private:
    Callback m_init;
    Callback m_setWon;
    Callback m_setLost;
    Callback m_checkWinOrFail;
    IntCallback m_jumpToSelectYourNextConquest;
    IntCallback m_goingToWinLoseBrief;
};
