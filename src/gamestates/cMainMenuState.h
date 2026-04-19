#pragma once

#include "cGameState.h"
#include "controls/cKeyboardEvent.h"
#include "utils/cRectangle.h"
#include "sMouseEvent.h"

#include <memory>

class Texture;
class cTextDrawer;
class cGameInterface;
class GuiButton;
class GuiWindow;

class cMainMenuState : public cGameState {
public:
    explicit cMainMenuState(sGameServices* services);
    ~cMainMenuState() override;

    void thinkFast() override;
    void draw() const override;

    void onNotifyMouseEvent(const s_MouseEvent &event) override;
    void onNotifyKeyboardEvent(const cKeyboardEvent &event) override;

    eGameStateType getType() override;

private:
    cGameSettings * m_settings = nullptr;
    cTextDrawer* m_textDrawer = nullptr;
    cGameInterface* m_interface = nullptr;    

    int logoX;
    int logoY;

    int mainMenuFrameX;
    int mainMenuFrameY;
    int mainMenuWidth;
    int mainMenuHeight;

    cRectangle sdl2power;

    std::unique_ptr<GuiWindow> gui_window;
    std::unique_ptr<GuiButton> gui_btn_credits;

    Texture *bmp_D2TM_Title;
    Texture *backGroundDebug = nullptr;
};
