/**
 * @file MentatFlow.hpp
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
 * Narrow entry point to the mentat related flow of the game (preparing the mentat, leaving it).
 * Holds callbacks so it does not depend on cGame.
 */
class MentatFlow {
public:
    using HouseCallback = std::function<void(int)>;
    using Callback = std::function<void()>;

    MentatFlow(HouseCallback prepareToTellAboutHouse, Callback prepareForPlayer, Callback leave)
        : m_prepareToTellAboutHouse(std::move(prepareToTellAboutHouse)),
          m_prepareForPlayer(std::move(prepareForPlayer)),
          m_leave(std::move(leave)) {
    }

    void prepareToTellAboutHouse(int house) const {
        m_prepareToTellAboutHouse(house);
    }

    void prepareForPlayer() const {
        m_prepareForPlayer();
    }

    void leave() const {
        m_leave();
    }

private:
    HouseCallback m_prepareToTellAboutHouse;
    Callback m_prepareForPlayer;
    Callback m_leave;
};
