#pragma once

#include "AbstractMentat.h"

class HarkonnenMentat : public AbstractMentat {
public:
    HarkonnenMentat(GameContext* ctx, bool allowMissionSelect);
    void think();
private:
    void draw_mouth();
    void draw_eyes();
    void draw_other();
    void draw();
};
