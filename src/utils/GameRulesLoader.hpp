/**
 * @file GameRulesLoader.hpp
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

#include <string>

class cInfoContext;
struct sGameServices;

// Loads the game rules (structures.ini and units.ini) into the info tables
class GameRulesLoader {
public:
    explicit GameRulesLoader(sGameServices* services);
    // Read structures.ini and units.ini (in the same directory as the given game.ini)
    void installGame(const std::string& filename);

private:
    void installStructures(const std::string& filename);
    void installUnits(const std::string& filename);

    cInfoContext* m_infos = nullptr;
};
