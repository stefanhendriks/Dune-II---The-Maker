#pragma once

#include <string>
#include "utils/Color.hpp"
#include "include/eNotificationType.h"


std::string eNotificationTypeString(const eNotificationType &type);

class cPlayerNotification {
public:
    cPlayerNotification(const std::string &msg, eNotificationType type);

    void thinkFast();

    bool isVisible() const {
        return m_TIMER > 0;
    }

    const std::string &getMessage() const;

    int getTimer() const {
        return m_TIMER;
    }

    Color getColor() const;

private:
    std::string m_msg;
    int m_TIMER;
    eNotificationType m_type;
};
