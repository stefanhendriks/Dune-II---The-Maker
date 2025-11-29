#pragma once

#include "cGameState.h"
#include "controls/cKeyboardEvent.h"
#include "drawers/cTextDrawer.h"
#include "sMouseEvent.h"

class Texture;
class cGame;



class cGamePlaying : public cGameState {
public:
    explicit cGamePlaying(cGame &theGame, GameContext* ctx);
    ~cGamePlaying() override;

    void thinkFast() override;
    void draw() const override;

    void onNotifyMouseEvent(const s_MouseEvent &event) override;
    void onNotifyKeyboardEvent(const cKeyboardEvent &event) override;

    eGameStateType getType() override;
private:

};
