#pragma once

#include "cGameState.h"

class cGameSettings;
class cGameInterface;
class cGameObjectContext;
class cMouse;

class cVideoState : public cGameState {
public:
    explicit cVideoState(sGameServices* services);
    ~cVideoState() override;

    void thinkFast() override;
    void thinkSlow() override;
    void draw() const override;

    void onNotifyMouseEvent(const s_MouseEvent &event) override;
    void onNotifyKeyboardEvent(const cKeyboardEvent &event) override;

    eGameStateType getType() override;

private:
    cGameSettings* m_settings = nullptr;
    cGameInterface* m_interface = nullptr;
    cGameObjectContext* m_objets = nullptr;
    cMouse* m_mouse = nullptr;
};

