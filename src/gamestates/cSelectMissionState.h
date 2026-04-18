#pragma once

#include "cGameState.h"
#include "controls/cKeyboardEvent.h"
#include "sMouseEvent.h"

#include <memory>

struct SDL_Surface;
class cGame;
class GameContext;
class cTextDrawer;
class GuiWindow;
class cGameSettings;
class cGameInterface;

class cSelectMissionState : public cGameState {
public:
    cSelectMissionState(cGame &theGame, sGameServices* services, int prevState);
    ~cSelectMissionState() override;

    void thinkFast() override;
    void draw() const override;

    void onNotifyMouseEvent(const s_MouseEvent &event) override;
    void onNotifyKeyboardEvent(const cKeyboardEvent &event) override;

    eGameStateType getType() override;

private:
    cTextDrawer *m_textDrawer = nullptr;
    cGameSettings* m_settings = nullptr;
    cGameInterface* m_interface = nullptr;
    std::unique_ptr<GuiWindow> gui_window;
};
