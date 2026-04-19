#pragma once

#include "cGameState.h"
#include "controls/cKeyboardEvent.h"
#include "sMouseEvent.h"

class Texture;

class cGameSettings;
class cGameInterface;

enum class Outcome : char {Win, Lose};


class cWinLoseState : public cGameState {
public:
    explicit cWinLoseState(sGameServices* services, Outcome value);
    ~cWinLoseState() override;

    void thinkFast() override;
    void draw() const override;

    void onNotifyMouseEvent(const s_MouseEvent &event) override;
    void onNotifyKeyboardEvent(const cKeyboardEvent &event) override;

    eGameStateType getType() override;

private:
    cGameSettings* m_settings = nullptr;
    cGameInterface* m_interface = nullptr;
    Texture *m_backgroundTexture = nullptr;
    Texture *m_tex = nullptr;
    Outcome m_statement;
    void onMouseLeftButtonClicked(const s_MouseEvent &event) const;
};
