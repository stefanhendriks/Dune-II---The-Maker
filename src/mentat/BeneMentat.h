#pragma once

#include "AbstractMentat.h"

class cTextDrawer;
/**
 * This is the mentat that is used for house selection. (Bene Geserit)
 */
class BeneMentat : public AbstractMentat {
public:
    BeneMentat();
    void think() override;
private:
    void draw_mouth() override;
    void draw_eyes() override;
    void draw_other() override;
    void draw() override;
    std::shared_ptr<cTextDrawer> textDrawer;
};
