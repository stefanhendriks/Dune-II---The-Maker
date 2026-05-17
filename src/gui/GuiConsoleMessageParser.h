#pragma once

#include <string>

class cNotificationArea;

class GuiConsoleMessageParser {
public:
    explicit GuiConsoleMessageParser(cNotificationArea* notificationArea);

    void submit(const std::string& text) const;

private:
    std::string normalize(const std::string& text) const;

    cNotificationArea* m_notificationArea = nullptr;
};
