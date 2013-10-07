#include "ActionManager.hpp"
#include "Game.hpp"

ActionManager::ActionManager(Game& theParent):
  parent(theParent),
  shouldDeselect(false)
{
  //thor Actions here

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
  actionMap.update(parent.screen);

  actionMap.invokeCallbacks(system, &(parent.screen));

  if (shouldDeselect){
    shouldDeselect = false;
    system.clearConnections("orderMove");
  }
}

void ActionManager::trigger(const std::string &which)
{
  system.triggerEvent(thor::ActionContext<std::string>(&parent.screen, nullptr, which));
}

void ActionManager::connect(const std::string &which, const ActionManager::Listener &listener)
{
  system.connect(which, listener);
}

void ActionManager::selectUnit(Unit &unit)
{
  unit.select();
  system.connect("orderMove", [this, &unit](thor::ActionContext<std::string> context){
    unit.orderMove(parent.screen.mapPixelToCoords(parent.mouse.getHotspot(*context.event), parent.camera));
  });
  parent.mouse.setType(Mouse::Type::Move); //at least one unit selected...
}
