#include "gui/GuiConsole/GuiConsoleMessageParser.h"

#include "game/cNotificationArea.h"
#include "include/eNotificationType.h"
#include "include/cAssert.h"

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

std::string toLower(std::string text)
{
    std::transform(text.begin(), text.end(), text.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });
    return text;
}
}

GuiConsoleMessageParser::GuiConsoleMessageParser(cNotificationArea* notificationArea)
    : m_notificationArea(notificationArea)
{
    d2tm_assert(m_notificationArea != nullptr);
}

std::string GuiConsoleMessageParser::normalize(const std::string& text) const
{
    return toLower(trim(text));
}

void GuiConsoleMessageParser::submit(const std::string& text)
{
    const std::string normalizedText = normalize(text);
    if (normalizedText.empty()) {
        return;
    }

    if (!verifyOddWordCommand(normalizedText)) {
        m_notificationArea->addNotification(std::format("Malformed command: {}", text), eNotificationType::BAD);
        return;
    }

    parseCommand(normalizedText);

    // if (m_command.empty()) {
    //     m_notificationArea->addNotification(std::format("Unknown command: {}", text), eNotificationType::BAD);
    //     return;
    // }

    if (m_command == "help") {
        m_notificationArea->addNotification("Help mode.", eNotificationType::NEUTRAL);
        return;
    }

    m_notificationArea->addNotification(std::format("> {}", normalizedText), eNotificationType::NEUTRAL);
}


void GuiConsoleMessageParser::parseCommand(const std::string& text)
{
   	std::istringstream commandstr( text );

    std::string key, value;
	commandstr >> m_command;

    while (commandstr >> key >> value ) {
		m_arguments[key] = value;
	}
}

bool GuiConsoleMessageParser::verifyOddWordCommand(const std::string& text) const
{
    int compteur = 0;
    bool dansMot = false;

    for (char c : text) {
        if (c != ' ' && !dansMot) {
            compteur++;
            dansMot = true;
        }
        else if (c == ' ') {
            dansMot = false;
        }
    }
    return (compteur % 2) == 1;
}