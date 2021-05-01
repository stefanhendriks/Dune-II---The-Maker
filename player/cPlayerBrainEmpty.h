#ifndef D2TM_CPLAYERBRAINEMPTY_H
#define D2TM_CPLAYERBRAINEMPTY_H

#include "cPlayerBrain.h"

class cPlayerBrainEmpty : public cPlayerBrain {

public:
    cPlayerBrainEmpty(cPlayer * player);
    ~cPlayerBrainEmpty();

    void think();

};


#endif //D2TM_CPLAYERBRAINEMPTY_H
