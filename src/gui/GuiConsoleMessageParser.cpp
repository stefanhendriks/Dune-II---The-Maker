#include "gui/GuiConsoleMessageParser.h"

#include "game/cNotificationArea.h"
#include "include/eNotificationType.h"

#include <algorithm>
#include <cctype>
#include <format>

namespace {
std::string trim(const std::string& text)
{
    const auto first = std::find_if_not(text.begin(), text.end(), [](unsigned char c) {
        return std::isspace(c) != 0;
    });

    const auto last = std::find_if_not(text.rbegin(), text.rend(), [](unsigned char c) {
        return std::isspace(c) != 0;
    }).base();

    if (first >= last) {
        return {};
    }

    return std::string(first, last);
}
}

GuiConsoleMessageParser::GuiConsoleMessageParser(cNotificationArea* notificationArea)
    : m_notificationArea(notificationArea)
{
}

std::string GuiConsoleMessageParser::normalize(const std::string& text) const
{
    return trim(text);
}

void GuiConsoleMessageParser::submit(const std::string& text) const
{
    const std::string normalizedText = normalize(text);
    if (normalizedText.empty() || !m_notificationArea) {
        return;
    }

    m_notificationArea->addNotification(std::format("> {}", normalizedText), eNotificationType::NEUTRAL);
}
