#pragma once

#include "cAbstractMentat.h"

class cHarkonnenMentat : public cAbstractMentat {
private:
    void draw_mouth();
    void draw_eyes();
    void draw_other();
    void draw();
    void interact();

public:
    cHarkonnenMentat(bool allowMissionSelect);

    void think();
};
