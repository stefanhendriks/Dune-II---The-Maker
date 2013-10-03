#ifndef ACTIONMANAGER_H
#define ACTIONMANAGER_H
#include <Thor/Input.hpp>

class Game;
class Unit;

class ActionManager
{
public:
    ActionManager(Game *theParent);

    void update();

private:
    Game* parent;

    thor::ActionMap<std::string> actionMap;
    thor::ActionMap<std::string>::CallbackSystem system;

    bool shouldDeselect;

    void selectUnit(Unit &unit);
};

#endif // ACTIONMANAGER_H
