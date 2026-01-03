#pragma once

#include "cGameState.h"
#include "controls/cKeyboardEvent.h"
#include "drawers/cTextDrawer.h"
#include "gui/GuiWindow.h"
#include "sMouseEvent.h"

struct SDL_Surface;
class cGame;
class GameContext;

class cNewMapEditorState : public cGameState {
public:
    explicit cNewMapEditorState(cGame &theGame, GameContext *ctx);
    ~cNewMapEditorState() override;

    void thinkFast() override;
    void draw() const override;

    void onNotifyMouseEvent(const s_MouseEvent &event) override;
    void onNotifyKeyboardEvent(const cKeyboardEvent &event) override;

    eGameStateType getType() override;

private:
    cTextDrawer* m_textDrawer = nullptr;
    GuiWindow *m_guiWindow;
    void constructWindow();
};
