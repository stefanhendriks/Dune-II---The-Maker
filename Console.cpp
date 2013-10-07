#include "Console.hpp"
#include "Game.hpp"
#include <boost/algorithm/string.hpp>

Console::Console(Game &theParent):
    chatLayout(sfg::Box::Create(sfg::Box::VERTICAL, 5.f)),
    chatAreaLayout(sfg::Box::Create(sfg::Box::VERTICAL)),
    chatEntry(sfg::Entry::Create()),
    chatWindow(sfg::ScrolledWindow::Create()),
    parent(theParent)
{
    sfg::Window::Ptr window(sfg::Window::Create(sfg::Window::SHADOW));

    chatWindow->SetRequisition(sf::Vector2f(400.f,400.f));
    chatWindow->SetScrollbarPolicy( sfg::ScrolledWindow::HORIZONTAL_AUTOMATIC | sfg::ScrolledWindow::VERTICAL_AUTOMATIC );
    chatWindow->AddWithViewport(chatAreaLayout);

    chatLayout->Pack(chatWindow);
    chatLayout->Pack(chatEntry);

    chatEntry->GetSignal(sfg::Entry::OnKeyPress).Connect(&Console::dataReady, this);

    window->Add(chatLayout);

    desktop.LoadThemeFromFile("gui.theme");
    //desktop.SetProperty

    desktop.Add(window);
    chatEntry->GrabFocus();
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

void Console::dataReady()
{
    if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Return)) return; //this is cheap
    std::string toProcess(chatEntry->GetText());
    if (toProcess.empty()) return;
    chatEntry->SetText("");

    std::vector<std::string> tokens;
    boost::split(tokens, toProcess, boost::is_any_of(" "));

    if (tokens[0]=="add"){
        if (tokens.size() != 6){
            toConsole("Incorrect number of arguments.");
            return;
        }
        //Unit* toAdd(parent.unitRepository->create(std::stoi(tokens[1]), std::stoi(tokens[2]), std::stoi(tokens[3]), std::stoi(tokens[4]), std::stoi(tokens[5])));
        //parent->units.emplace_back(toAdd);
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

