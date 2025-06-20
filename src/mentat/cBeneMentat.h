#pragma once

#include "cAbstractMentat.h"

/**
 * This is the mentat that is used for house selection. (Bene Geserit)
 */
class cBeneMentat : public cAbstractMentat {
private:
    void draw_mouth() override;
    void draw_eyes() override;
    void draw_other() override;
    void draw() override;
    void interact() override;

public:
    cBeneMentat();

    void think() override;
};
