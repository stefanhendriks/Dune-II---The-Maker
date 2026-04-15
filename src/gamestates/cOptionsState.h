#pragma once

#include "cGameState.h"
#include "controls/cKeyboardEvent.h"
#include "sMouseEvent.h"
#include "include/Texture.hpp"

#include <memory>

struct SDL_Surface;
class cGame;
class GameContext;
class cTextDrawer;
class GuiWindow;

class cOptionsState : public cGameState {
public:
    explicit cOptionsState(cGame &theGame, sGameServices* services, int prevState);
    ~cOptionsState() override;

    void thinkFast() override;
    void draw() const override;

    void onNotifyMouseEvent(const s_MouseEvent &event) override;
    void onNotifyKeyboardEvent(const cKeyboardEvent &event) override;

    eGameStateType getType() override;

    void setPrevState(int prevState);
    void refresh();

private:
    cTextDrawer* m_textDrawer = nullptr;

    int m_prevState;

    std::unique_ptr<GuiWindow> m_guiWindow;

    void constructWindow(int prevState);
    Texture *m_backgroundTexture = nullptr;
};
