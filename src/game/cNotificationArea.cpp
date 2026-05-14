#include "cNotificationArea.h"

#include "drawers/cTextDrawer.h"

#include <algorithm>

void cNotificationArea::clear()
{
    m_notifications.clear();
}

void cNotificationArea::addNotification(const std::string &msg, eNotificationType type)
{
    m_notifications.push_back(cPlayerNotification(msg, type));
    std::sort(m_notifications.begin(), m_notifications.end(), [](const cPlayerNotification &lhs, const cPlayerNotification &rhs) {
        return lhs.getTimer() > rhs.getTimer();
    });
}

void cNotificationArea::thinkFast()
{
    for (auto &notification : m_notifications) {
        notification.thinkFast();
    }

    m_notifications.erase(
        std::remove_if(
            m_notifications.begin(),
            m_notifications.end(),
            [](cPlayerNotification notification) {
                return !notification.isVisible();
            }),
        m_notifications.end());
}

void cNotificationArea::draw(const cTextDrawer *textDrawer, int x, int y) const
{
    if (!textDrawer) return;

    for (const auto &notification : m_notifications) {
        textDrawer->drawText(x, y, notification.getColor(), notification.getMessage());
        y -= 15;
    }
}

std::vector<cPlayerNotification> &cNotificationArea::getNotifications()
{
    return m_notifications;
}

const std::vector<cPlayerNotification> &cNotificationArea::getNotifications() const
{
    return m_notifications;
}