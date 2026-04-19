#pragma once

#include "cGameState.h"
#include "controls/cKeyboardEvent.h"
#include "sMouseEvent.h"

#include <memory>
#include <vector>

class cGame;
class cTextDrawer;
class cGameInterface;
class GuiWindow;
class GuiCycleButton;
class GuiTextInput;


class cNewMapEditorState : public cGameState {
public:
    explicit cNewMapEditorState(sGameServices* services);
    ~cNewMapEditorState() override;

    void thinkFast() override;
    void draw() const override;

    void onNotifyMouseEvent(const s_MouseEvent &event) override;
    void onNotifyKeyboardEvent(const cKeyboardEvent &event) override;

    eGameStateType getType() override;

private:
    cTextDrawer* m_textDrawer = nullptr;
    cGameSettings* m_settings = nullptr;
    cGameInterface* m_interface = nullptr;
    std::unique_ptr<GuiWindow> m_guiWindow;
    void constructWindow();
    GuiTextInput* m_inputName = nullptr;
    GuiTextInput* m_inputAuthor = nullptr;
    GuiTextInput* m_inputDescription = nullptr;
    GuiCycleButton* m_cycleWidth = nullptr;
    GuiCycleButton* m_cycleHeight = nullptr;
    std::vector<int> m_sizesMap = {16, 24, 32, 64, 96, 128, 160, 192, 224, 256};
};
