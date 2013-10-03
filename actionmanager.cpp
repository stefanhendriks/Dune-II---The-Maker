#include "actionmanager.h"
#include "game.h"

ActionManager::ActionManager(Game* theParent):
    parent(theParent)
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

    typedef thor::ActionContext<std::string> actionContext;

    system.connect("close", [this](actionContext){parent->playing = false;});

    system.connect("boxRelease", [this](actionContext){
      for (auto& unit : parent->units){
        if (parent->box.intersects(unit.getBounds())){
          selectUnit(unit);
        }
      }
      parent->box.clear();
    });

    system.connect("boxStart", [this](actionContext context){
      sf::Vector2f toSet = parent->screen.mapPixelToCoords(parent->mouse.getHotspot(*context.event), parent->camera);
      parent->box.setTopLeft(toSet);
    });

    system.connect("singleSelect", [this](actionContext context){
      sf::Vector2f toCheck = parent->screen.mapPixelToCoords(parent->mouse.getHotspot(*context.event), parent->camera);
      for (auto& unit : parent->units){
        if (unit.getBounds().contains(toCheck))
          selectUnit(unit);
      }
    });

    system.connect("deselectAll", [this](actionContext){
      shouldDeselect = true;
      parent->mouse.setType(Mouse::Type::Default);
      for (auto& unit : parent->units)
        unit.unselect();
    });

    system.connect("boxDrag", [this](actionContext){
      parent->box.setBottomRight(parent->screen.mapPixelToCoords(sf::Mouse::getPosition(parent->screen),parent->camera));
    });

    const float cameraSpeed = 15.f;

    system.connect("cameraLeft", [this, cameraSpeed](actionContext) {parent->camera.move(-cameraSpeed, 0.f);});
    system.connect("cameraRight", [this, cameraSpeed](actionContext){parent->camera.move(cameraSpeed, 0.f); });
    system.connect("cameraUp", [this, cameraSpeed](actionContext)   {parent->camera.move(0.f, -cameraSpeed);});
    system.connect("cameraDown", [this, cameraSpeed](actionContext) {parent->camera.move(0.f, cameraSpeed); });
}

void ActionManager::update()
{
    actionMap.update(parent->screen);

    actionMap.invokeCallbacks(system, &(parent->screen));

    if (shouldDeselect){
      shouldDeselect = false;
      system.clearConnections("orderMove");
    }
}

void ActionManager::selectUnit(Unit &unit)
{
  unit.select();
  system.connect("orderMove", [this, &unit](thor::ActionContext<std::string> context){
    unit.order_move(parent->screen.mapPixelToCoords(parent->mouse.getHotspot(*context.event), parent->camera));
  });
  parent->mouse.setType(Mouse::Type::Move); //at least one unit selected...
}
