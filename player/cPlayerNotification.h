#ifndef D2TM_CPLAYERNOTIFICATION_H
#define D2TM_CPLAYERNOTIFICATION_H

#include <string>

enum eNotificationType {
    NEUTRAL,    // generic message, in white
    PRIORITY,   // a more important message, yellow
    BAD         // bad news, in red
};

static const char* eNotificationTypeString(const eNotificationType &type) {
    switch (type) {
        case eNotificationType::NEUTRAL: return "NEUTRAL";
        case eNotificationType::PRIORITY: return "PRIORITY";
        case eNotificationType::BAD: return "BAD";
        default:
            assert(false && "Unknown eNotificationType?");
            break;
    }
    return "";
}

class cPlayerNotification {
public:
    cPlayerNotification(std::string msg, eNotificationType type);

    void thinkFast();

    bool isVisible() { return TIMER > 0; }

    std::string & getMessage();

    int getTimer() const { return TIMER; }

    int getColor();

private:
    std::string msg;
    int TIMER;
    int initialDuration;
    eNotificationType type;
};

#endif //D2TM_CPLAYERNOTIFICATION_H
