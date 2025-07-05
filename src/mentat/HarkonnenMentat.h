#pragma once

#include "AbstractMentat.h"

class HarkonnenMentat : public AbstractMentat {
public:
    HarkonnenMentat(bool allowMissionSelect);
    void think();
private:
    void draw_mouth();
    void draw_eyes();
    void draw_other();
    void draw();
    // void interact();
};
