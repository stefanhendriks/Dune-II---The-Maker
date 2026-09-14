/**
 * @file cNewMapEditorState.h
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

#include "cGameState.h"
#include "controls/cKeyboardEvent.h"
#include "controls/sMouseEvent.h"

#include <memory>
#include <vector>


class cTextDrawer;
class cGameInterface;
class GuiWindow;
class GuiCycleButton;
class GuiTextInput;
class cPreviewMaps;


class cNewMapEditorState : public cGameState {
public:
    explicit cNewMapEditorState(sGameServices* services);
    ~cNewMapEditorState() override;

    void thinkFast() override;
    void draw() const override;

    void onNotifyMouseEvent(const s_MouseEvent &event) override;
    void onNotifyKeyboardEvent(const cKeyboardEvent &event) override;
    bool isKeyboardCapturedByUi() const override;

    eGameStateType getType() override;

private:
    cTextDrawer* m_textDrawer = nullptr;
    cGameSettings* m_settings = nullptr;
    cGameInterface* m_interface = nullptr;
    std::unique_ptr<GuiWindow> m_guiWindow;
    cPreviewMaps* m_previewMap = nullptr;
    void constructWindow();
    GuiTextInput* m_inputName = nullptr;
    GuiTextInput* m_inputAuthor = nullptr;
    GuiTextInput* m_inputDescription = nullptr;
    GuiCycleButton* m_cycleWidth = nullptr;
    GuiCycleButton* m_cycleHeight = nullptr;

    void constructEmptyMap();

    std::vector<int> m_sizesMap = {16, 24, 32, 48, 64, 80, 96, 112, 128, 144, 160, 176, 192};
};
