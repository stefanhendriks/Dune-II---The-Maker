#pragma once

#include "cGameState.h"
#include <memory>

class cGameSettings;
class cGameInterface;
class cGameObjectContext;
class cMouse;
class Graphics;
class Texture;

class cVideoState : public cGameState {
public:
    explicit cVideoState(sGameServices* services);
    ~cVideoState() override;

    void thinkFast() override;
    void thinkNormal() override;
    void draw() const override;

    void onNotifyMouseEvent(const s_MouseEvent &event) override;
    void onNotifyKeyboardEvent(const cKeyboardEvent &event) override;

    eGameStateType getType() override;

private:
    cGameSettings* m_settings = nullptr;
    cGameInterface* m_interface = nullptr;
    cGameObjectContext* m_objets = nullptr;
    cMouse* m_mouse = nullptr;
    std::unique_ptr<Graphics> gfxmovie;
    int m_movieFrame = 0;
    int m_timerFrame = 0;
    int offsetX;
    int offsetY;
    Texture* m_currentFrame = nullptr;
};

