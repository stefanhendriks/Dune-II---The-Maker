#pragma once

#include "cAbstractMentat.h"

class cOrdosMentat : public cAbstractMentat {
private:
    void draw_mouth();
    void draw_eyes();
    void draw_other();
    void draw();
    void interact();

public:
    cOrdosMentat(bool allowMissionSelect);

    void think();
};
