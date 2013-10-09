#include "ActionManager.hpp"
#include "Game.hpp"

ActionManager::ActionManager(sf::RenderWindow& screen):
  screen(screen)
{
  actionMap["boxStart"] = thor::Action(sf::Mouse::Left, thor::Action::PressOnce);
  actionMap["orderMove"] = thor::Action(sf::Mouse::Left, thor::Action::PressOnce);
  actionMap["singleSelect"] = thor::Action(sf::Mouse::Left, thor::Action::PressOnce);
  actionMap["boxDrag"] = thor::Action(sf::Mouse::Left, thor::Action::Hold);
  actionMap["boxRelease"] = thor::Action(sf::Mouse::Left, thor::Action::ReleaseOnce);
  actionMap["deselectAll"] = thor::Action(sf::Mouse::Right, thor::Action::PressOnce);
  actionMap["close"] = thor::Action(sf::Event::Closed) || thor::Action(sf::Keyboard::Q, thor::Action::PressOnce);
  actionMap["cameraLeft"] = thor::Action(sf::Keyboard::Left, thor::Action::Hold);
  actionMap["cameraRight"] = thor::Action(sf::Keyboard::Right, thor::Action::Hold);
  actionMap["cameraUp"] = thor::Action(sf::Keyboard::Up, thor::Action::Hold);
  actionMap["cameraDown"] = thor::Action(sf::Keyboard::Down, thor::Action::Hold);
}

void ActionManager::update()
{
  actionMap.update(screen);

  actionMap.invokeCallbacks(system, &(screen));

  while (!toDisconnect.empty()){
    system.clearConnections(toDisconnect.front());
    toDisconnect.pop_front();
  }

}

void ActionManager::trigger(const std::string &which)
{
  system.triggerEvent(thor::ActionContext<std::string>(&screen, nullptr, which));
}

void ActionManager::connect(const std::string &which, const ActionManager::Listener &listener)
{
  system.connect(which, listener);
}

void ActionManager::disconnect(const std::string &which)
{
  toDisconnect.emplace_back(which);
}
