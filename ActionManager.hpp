#ifndef ACTIONMANAGER_H
#define ACTIONMANAGER_H
#include <Thor/Input.hpp>

class Game;
class Unit;

class ActionManager
{
  public:
    typedef thor::ActionMap::CallbackSystem::Listener Listener;

    ActionManager(Game &theParent);

    void update();
    void trigger(const std::string& which);
    void connect(const std::string& which, const Listener& listener);

  private:
    Game& parent;

    thor::ActionMap<std::string> actionMap;
    thor::ActionMap<std::string>::CallbackSystem system;

    bool shouldDeselect;

    void selectUnit(Unit &unit);
};

#endif // ACTIONMANAGER_H
