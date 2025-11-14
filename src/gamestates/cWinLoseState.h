#pragma once

#include "cGameState.h"
#include "controls/cKeyboardEvent.h"
#include "drawers/cTextDrawer.h"
#include "sMouseEvent.h"

class Texture;
class cGame;

enum class Outcome : char {Win, Lose};


class cWinLoseState : public cGameState {
public:
    explicit cWinLoseState(cGame &theGame, GameContext* ctx, Outcome value);
    ~cWinLoseState() override;

    void thinkFast() override;
    void draw() const override;

    void onNotifyMouseEvent(const s_MouseEvent &event) override;
    void onNotifyKeyboardEvent(const cKeyboardEvent &event) override;

    eGameStateType getType() override;

private:
    Texture *m_backgroundTexture;
    void onMouseLeftButtonClicked(const s_MouseEvent &event) const;
};
