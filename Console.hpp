#ifndef CONSOLE_HPP
#define CONSOLE_HPP
#include <SFGUI/SFGUI.hpp>
#include <map>
#include "Unit.hpp"
#include "Player.hpp"

class Console
{
public:
    Console(MessageSystem& messages, const std::vector<Player>& players);

    void update(sf::Time dt);
    void handleEvent(sf::Event event);
    void display(sf::RenderWindow& screen);
    void toggle();

private:
    sfg::SFGUI sfgui;
    sfg::Desktop desktop;
    sfg::Box::Ptr chatLayout;
    sfg::Box::Ptr chatAreaLayout;
    sfg::Entry::Ptr chatEntry;
    sfg::ScrolledWindow::Ptr chatWindow;
    sfg::Window::Ptr window;

    MessageSystem& messages;

    void dataReady();
    void toConsole(std::string toWrite);
    void autoscroll();

    std::map<std::string, Unit::Type> unitMap;
    const std::vector<Player>& players;

};

#endif // CONSOLE_HPP
