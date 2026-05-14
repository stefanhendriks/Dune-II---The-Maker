#pragma once

#include "gameobjects/players/cPlayerNotification.h"
#include <string>
#include <vector>

class cTextDrawer;

class NotificationArea {
public:
    void clear();

    void addNotification(const std::string &msg, eNotificationType type);

    void thinkFast();

    void draw(const cTextDrawer *textDrawer, int x, int y) const;

    std::vector<cPlayerNotification> &getNotifications();
    const std::vector<cPlayerNotification> &getNotifications() const;

private:
    std::vector<cPlayerNotification> m_notifications;
};