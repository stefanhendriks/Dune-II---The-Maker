#ifndef D2TM_CPLAYERBRAINFREMENSUPERWEAPON_H
#define D2TM_CPLAYERBRAINFREMENSUPERWEAPON_H

#include "cPlayerBrain.h"

class cPlayerBrainFremenSuperWeapon : public cPlayerBrain {

public:
    cPlayerBrainFremenSuperWeapon(cPlayer * player);
    ~cPlayerBrainFremenSuperWeapon();

    void think();

};


#endif //D2TM_CPLAYERBRAINFREMENSUPERWEAPON_H
