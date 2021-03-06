#ifndef D2TM_CHARKONNENMENTAT_H
#define D2TM_CHARKONNENMENTAT_H

#include "cAbstractMentat.h"

class cHarkonnenMentat : public cAbstractMentat {

private:

    void draw_mouth();
    void draw_eyes();
    void draw_other();
    void draw();
    void interact();

public:
    cHarkonnenMentat();
    ~cHarkonnenMentat();

    void think();
};


#endif //D2TM_CHARKONNENMENTAT_H
