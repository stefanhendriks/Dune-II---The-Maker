#ifndef D2TM_CBENEMENTAT_H
#define D2TM_CBENEMENTAT_H

#include "cAbstractMentat.h"

class cBeneMentat : public cAbstractMentat {

private:

    void draw_mouth();
    void draw_eyes();
    void draw_other();
    void draw();
    void interact();

public:
    cBeneMentat();
    ~cBeneMentat();

    void think();
};


#endif //D2TM_CBENEMENTAT_H
