#include "cPlayerNotification.h"

cPlayerNotification::cPlayerNotification(std::string msg) {
    this->msg = msg;
    TIMER = (1000/5) * 7;
}

void cPlayerNotification::thinkFast() {
    TIMER--;
}

std::string & cPlayerNotification::getMessage() {
    return msg;
}
