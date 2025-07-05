#pragma once

#include "AbstractMentat.h"

class OrdosMentat : public AbstractMentat {
public:
    OrdosMentat(bool allowMissionSelect);
    void think();
private:
    void draw_mouth();
    void draw_eyes();
    void draw_other();
    void draw();
    // void interact();
};
