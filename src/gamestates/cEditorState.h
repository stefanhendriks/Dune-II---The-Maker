#pragma once

#include "cGameState.h"
#include "controls/cKeyboardEvent.h"
#include "drawers/cTextDrawer.h"
#include "sMouseEvent.h"

class Texture;
class cGame;

class cEditorState : public cGameState {
public:
    explicit cEditorState(cGame &theGame, GameContext* ctx);
    ~cEditorState() override;

    void thinkFast() override;
    void draw() const override;

    void onNotifyMouseEvent(const s_MouseEvent &event) override;
    void onNotifyKeyboardEvent(const cKeyboardEvent &event) override;

    eGameStateType getType() override;
private:
};
