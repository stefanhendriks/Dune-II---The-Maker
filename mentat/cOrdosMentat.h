#ifndef D2TM_CORDOSMENTAT_H
#define D2TM_CORDOSMENTAT_H

#include "cAbstractMentat.h"

class cOrdosMentat : public cAbstractMentat {

private:

    void draw_mouth();
    void draw_eyes();
    void draw_other();
    void draw();
    void interact();

public:
    cOrdosMentat();
    ~cOrdosMentat();

    void think();
};


#endif //D2TM_CORDOSMENTAT_H
