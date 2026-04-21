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

    void setLog(cLog *log);
    cLog* getLog() const;

    void resetCache() const;
private:
    std::unique_ptr<GraphicsContext> m_graphicsContext;
    std::unique_ptr<cTimeManager> m_timeManager;
    std::unique_ptr<cSoundPlayer> m_soundPlayer;
    std::unique_ptr<TextContext> m_textContext;
    std::unique_ptr<SDLDrawer> m_SDLDrawer;
    std::unique_ptr<cGameInterface> m_gameInterface;
    cLog *m_log = nullptr;
};