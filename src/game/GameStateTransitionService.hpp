/**
 * @file GameStateTransitionService.hpp
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

class cGame;

class GameStateTransitionService {
public:
    explicit GameStateTransitionService(cGame &game);

    void setNextState(int state) const;
    void setNextStateWithFade(int state) const;

private:
    cGame &m_game;
};
