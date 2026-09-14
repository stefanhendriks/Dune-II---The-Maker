/**
 * @file GuiConsoleMessageParser.h
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
#include <map>

typedef std::map< std::string, std::string > stringHash;

class cNotificationArea;

class GuiConsoleMessageParser {
public:
    explicit GuiConsoleMessageParser(cNotificationArea* notificationArea);

    void submit(const std::string& text);

private:
    void parseCommand(const std::string& text);
    bool verifyOddWordCommand(const std::string& text) const;

    std::string normalize(const std::string& text) const;

    cNotificationArea* m_notificationArea = nullptr;
    std::string m_command;
    stringHash m_arguments;
};
