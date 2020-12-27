#ifndef D2TM_CATREIDESMENTAT_H
#define D2TM_CATREIDESMENTAT_H

#include "cAbstractMentat.h"

class cAtreidesMentat : public cAbstractMentat {

protected:

    void draw_mouth() override;
    void draw_eyes() override;
    void draw_other() override;
    void draw() override;
    void interact() override;

public:
    cAtreidesMentat();
    ~cAtreidesMentat() override;

    void think() override;
};


#endif //D2TM_CATREIDESMENTAT_H
