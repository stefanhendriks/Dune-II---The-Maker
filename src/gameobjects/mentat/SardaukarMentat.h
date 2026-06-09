#pragma once

#include "AbstractMentat.h"

class SardaukarMentat : public AbstractMentat {
public:
    SardaukarMentat(GameContext* ctx, bool allowMissionSelect);
    void think();
private:
    void draw_mouth();
    void draw_eyes();
    void draw_other();
    void draw();
};
