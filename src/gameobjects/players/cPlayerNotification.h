/**
 * @file cPlayerNotification.h
 *
 * Dune 2 - The Maker
 *
 * @author Stefan Hendriks & the D2TM Team
 * @www http://www.dune2themaker.com
 * @copyright Copyright (c) 2001 - 2026 D2TM Team
 * @license This software is released under the MIT License.
 *
 * Note: Dune 2 is a trademark of Westwood Studios/Electronic Arts.
 *
 * This is a non-commercial educational project.
 */

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
