#ifndef D2TM_CPLAYERNOTIFICATION_H
#define D2TM_CPLAYERNOTIFICATION_H

#include <string>

class cPlayerNotification {
public:
    cPlayerNotification(std::string msg);

    void thinkFast();

    bool isVisible() { return TIMER > 0; }

    std::string & getMessage();

private:
    std::string msg;
    int TIMER;
};

#endif //D2TM_CPLAYERNOTIFICATION_H
