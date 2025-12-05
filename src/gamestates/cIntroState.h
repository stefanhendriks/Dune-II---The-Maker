#pragma once

#include "cGameState.h"
#include <memory>

class Graphics;


class cIntroState : public cGameState {
public:
    cIntroState(cGame &game, GameContext* ctx);
    ~cIntroState() override;

    void thinkFast() override;
    void draw() const override;

    void onNotifyMouseEvent(const s_MouseEvent &event) override;
    void onNotifyKeyboardEvent(const cKeyboardEvent &event) override;

    eGameStateType getType() override;
private:
    std::shared_ptr<Graphics> gfxmovie;
    int TIMER_movie;
    int iMovieFrame;

    int offsetX;
    int offsetY;
    int movieTopleftX;
    int movieTopleftY;
};