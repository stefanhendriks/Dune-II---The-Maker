#ifndef CONSOLE_HPP
#define CONSOLE_HPP
#include <SFGUI/SFGUI.hpp>
#include <map>
#include "Unit.hpp"

class Console
{
public:
    Console();

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

    void dataReady();
    void toConsole(std::string toWrite);
    void autoscroll();

    std::map<std::string, Unit::Type> unitMap;

};

#endif // CONSOLE_HPP
