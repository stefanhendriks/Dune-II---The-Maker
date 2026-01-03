#pragma once

#include "cGameState.h"
#include "controls/cKeyboardEvent.h"
#include "drawers/cTextDrawer.h"
#include "gui/GuiWindow.h"
#include "gui/GuiCycleButton.h"
#include "gui/GuiTextInput.h"
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
    GuiTextInput* m_inputName = nullptr;
    GuiTextInput* m_inputAuthor = nullptr;
    GuiTextInput* m_inputDescription = nullptr;
    GuiCycleButton* m_cycleWidth = nullptr;
    GuiCycleButton* m_cycleHeight = nullptr;
    std::vector<int> m_sizesMap = {32, 64, 96, 128, 160, 192, 224, 256};
};
