#include "Console.hpp"
#include "Game.hpp"
#include <boost/algorithm/string.hpp>

Console::Console():
    chatLayout(sfg::Box::Create(sfg::Box::VERTICAL, 5.f)),
    chatAreaLayout(sfg::Box::Create(sfg::Box::VERTICAL)),
    chatEntry(sfg::Entry::Create()),
    chatWindow(sfg::ScrolledWindow::Create()),
    window(sfg::Window::Create(sfg::Window::SHADOW))
{
    chatWindow->SetRequisition(sf::Vector2f(750.f,100.f));
    chatWindow->SetScrollbarPolicy( sfg::ScrolledWindow::HORIZONTAL_AUTOMATIC | sfg::ScrolledWindow::VERTICAL_AUTOMATIC );
    chatWindow->AddWithViewport(chatAreaLayout);

    chatLayout->Pack(chatWindow);
    chatLayout->Pack(chatEntry);

    chatEntry->GetSignal(sfg::Entry::OnKeyPress).Connect(&Console::dataReady, this);

    window->Add(chatLayout);
    window->Show(false);

    desktop.LoadThemeFromFile("gui.theme");


    desktop.Add(window);
    chatEntry->GrabFocus();

    unitMap["trike"] = Unit::Type::Trike;
    unitMap["quad"] = Unit::Type::Quad;
    unitMap["devastator"] = Unit::Type::Devastator;
    unitMap["carryall"] = Unit::Type::Carryall;
    unitMap["frigate"] = Unit::Type::Frigate;
    unitMap["soldier"] = Unit::Type::Soldier;
}

void Console::update(sf::Time dt)
{
    desktop.Update(dt.asSeconds());
}

void Console::handleEvent(sf::Event event)
{
    desktop.HandleEvent(event);
}

void Console::display(sf::RenderWindow &screen)
{
    sfgui.Display(screen);
}

void Console::toggle()
{
    window->Show(!window->IsLocallyVisible());
}

void Console::dataReady()
{
    if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Return)) return; //this is cheap
    std::string toProcess(chatEntry->GetText());
    if (toProcess.empty()) return;
    chatEntry->SetText("");

    std::vector<std::string> tokens;
    boost::split(tokens, toProcess, boost::is_any_of(" "));

    if (tokens[0]=="add"){
        if (tokens.size() != 5){
            toConsole("Incorrect number of arguments.");
            return;
        }
        // todo: use events emitting for this?
        // parent.units.push_back(
        //   std::move(parent.unitRepository.create(
        //             unitMap[tokens[1]], parent.players[std::stoi(tokens[2])],
        //             sf::Vector2f(std::stoi(tokens[3]), std::stoi(tokens[4])), *parent.map)));

        toConsole("Unit added.");
    }else{
        toConsole("Bad command.");
        return;
    }
}

void Console::toConsole(std::string toWrite)
{
    sfg::Container::WidgetsList lines(chatAreaLayout->GetChildren());
    if (lines.size()>120) //if more, pop front
        chatAreaLayout->Remove(lines.front());


    sfg::Label::Ptr toAdd(sfg::Label::Create(toWrite));
    toAdd->SetAlignment(sf::Vector2f(0.f,0.f));
    chatAreaLayout->PackEnd(toAdd);
    autoscroll();
}

void Console::autoscroll()
{
    sfg::Adjustment::Ptr toAdjust(chatWindow->GetVerticalAdjustment());
    toAdjust->SetValue(toAdjust->GetUpper());
}

