//
// Created by shendriks on 5/1/2021.
//

#ifndef D2TM_CPLAYERBRAIN_H
#define D2TM_CPLAYERBRAIN_H


class cPlayer;

class cPlayerBrain {

public:
    cPlayerBrain(cPlayer * player);
    virtual ~cPlayerBrain() = 0;

    virtual void think() = 0;

protected:
    cPlayer *player_;

private:


};


#endif //D2TM_CPLAYERBRAIN_H
