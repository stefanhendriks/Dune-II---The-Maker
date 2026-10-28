/**
 * @file cGameInterface.h
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
class cPlayer;
struct s_GameEvent;
struct s_PreviewMap;

class cGameInterface
{
public:
    cGameInterface(cGame* game);
    virtual ~cGameInterface() = default;

    void setupPlayers() const;

    void loadScenario() const;

    // void loadSkirmishMaps() const;
    void initSkirmish() const;
    // void loadMapFromEditor(s_PreviewMap *map) const;
    void setPlayerToInteractFor(cPlayer *pPlayer) const;
    void loadMapFromEditor(int iSkirmishMap) const;
    void drawTextFps() const;
    void drawTextTime() const;

private:
    cGame* m_game = nullptr;
};