#ifndef ACTIONMANAGER_H
#define ACTIONMANAGER_H
#include <Thor/Input.hpp>
#include <deque>

namespace sf{ class RenderWindow; }

class ActionManager
{
  public:
    typedef thor::ActionMap<std::string>::CallbackSystem::Listener Listener;

    ActionManager(sf::RenderWindow &screen);

    void update();
    void trigger(const std::string& which);
    void connect(const std::string& which, const Listener& listener);
    void disconnect(const std::string& which);

  private:
    sf::RenderWindow& screen;

    thor::ActionMap<std::string> actionMap;
    thor::ActionMap<std::string>::CallbackSystem system;

    std::deque<std::string> toDisconnect;
};

#endif // ACTIONMANAGER_H
