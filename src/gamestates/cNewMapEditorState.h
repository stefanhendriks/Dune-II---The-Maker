#pragma once

#include "cGameState.h"
#include "controls/cKeyboardEvent.h"
#include "gui/GuiWindow.h"
#include "gui/GuiCycleButton.h"
#include "gui/GuiTextInput.h"
#include "sMouseEvent.h"

#include <memory>

struct SDL_Surface;
class cGame;
class GameContext;
class cTextDrawer;

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
    std::unique_ptr<GuiWindow> m_guiWindow;
    void constructWindow();
    std::unique_ptr<GuiTextInput> m_inputName;
    std::unique_ptr<GuiTextInput> m_inputAuthor;
    std::unique_ptr<GuiTextInput> m_inputDescription;
    std::unique_ptr<GuiCycleButton> m_cycleWidth;
    std::unique_ptr<GuiCycleButton> m_cycleHeight;
    std::vector<int> m_sizesMap = {16, 24, 32, 64, 96, 128, 160, 192, 224, 256};
};
