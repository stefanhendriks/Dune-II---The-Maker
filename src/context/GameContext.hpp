/**
 * @file GameContext.hpp
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

#include <stdexcept>
#include <memory>

#include "context/GraphicsContext.hpp"
#include "context/TextContext.hpp"
#include "game/cGameSettings.h"

class cTimeManager;
class cSoundPlayer;
class SDLDrawer;
class cGameInterface;
class cLog;

class GameContext {
public:
    GameContext() = default;
    ~GameContext();

    void setGraphicsContext(std::unique_ptr<GraphicsContext> graphicsContext);
    GraphicsContext* getGraphicsContext() const;

    void setTimeManager(std::unique_ptr<cTimeManager> timeManager);
    cTimeManager* getTimeManager() const;

    void setSoundPlayer(std::unique_ptr<cSoundPlayer> soundPlayer);
    cSoundPlayer* getSoundPlayer() const;

    void setTextContext(std::unique_ptr<TextContext> textContext);
    TextContext* getTextContext() const;

    void setSDLDrawer(std::unique_ptr<SDLDrawer> SDLDrawer);
    SDLDrawer* getSDLDrawer() const;

    void setGameInterface(std::unique_ptr<cGameInterface> gameInterface);
    cGameInterface* getGameInterface() const;

    void resetCache() const;
private:
    std::unique_ptr<GraphicsContext> m_graphicsContext;
    std::unique_ptr<cTimeManager> m_timeManager;
    std::unique_ptr<cSoundPlayer> m_soundPlayer;
    std::unique_ptr<TextContext> m_textContext;
    std::unique_ptr<SDLDrawer> m_SDLDrawer;
    std::unique_ptr<cGameInterface> m_gameInterface;
};