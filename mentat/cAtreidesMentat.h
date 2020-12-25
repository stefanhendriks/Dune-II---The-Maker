#ifndef D2TM_CATREIDESMENTAT_H
#define D2TM_CATREIDESMENTAT_H

#include "cAbstractMentat.h"

class cAtreidesMentat : public cAbstractMentat {

protected:

    void draw_mouth();
    void draw_eyes();
    void draw_other();
    void draw();
    void interact();

public:
    cAtreidesMentat();
    ~cAtreidesMentat();

    void think();
};


#endif //D2TM_CATREIDESMENTAT_H
