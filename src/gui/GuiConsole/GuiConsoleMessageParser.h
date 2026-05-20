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
