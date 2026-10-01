#include <catch2/catch_test_macros.hpp>

#include <memory>

#include <SDL3/SDL.h>

#include "drawers/SDLDrawer.hpp"
#include "gui/GuiButton.h"

namespace {

class GuiButtonTestFixture {
public:
    GuiButtonTestFixture() {
        REQUIRE(SDL_Init(SDL_INIT_VIDEO));

        window = SDL_CreateWindow("d2tm-gui-tests", 128, 128, SDL_WINDOW_HIDDEN);
        REQUIRE(window != nullptr);

        renderer = SDL_CreateRenderer(window, nullptr);
        REQUIRE(renderer != nullptr);

        drawer = std::make_unique<SDLDrawer>(renderer);
    }

    ~GuiButtonTestFixture() {
        drawer.reset();

        if (renderer != nullptr) {
            SDL_DestroyRenderer(renderer);
            renderer = nullptr;
        }

        if (window != nullptr) {
            SDL_DestroyWindow(window);
            window = nullptr;
        }

        SDL_Quit();
    }

    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    std::unique_ptr<SDLDrawer> drawer;
};

} // namespace

TEST_CASE_METHOD(GuiButtonTestFixture, "GuiButton - hover and pressed states follow mouse movement and clicks", "[gui]")
{
    GuiButton button(drawer.get(), cRectangle(10, 20, 100, 30), "Play");
    int clickCount = 0;
    button.setOnLeftMouseButtonClickedAction([&]() {
        ++clickCount;
    });

    button.onNotifyMouseEvent({MOUSE_MOVED_TO, cPoint(20, 25)});
    REQUIRE(button.visualState() == GuiButton::VisualState::Hover);

    button.onNotifyMouseEvent({MOUSE_LEFT_BUTTON_PRESSED, cPoint(20, 25)});
    REQUIRE(button.visualState() == GuiButton::VisualState::Pressed);

    button.onNotifyMouseEvent({MOUSE_MOVED_TO, cPoint(200, 25)});
    REQUIRE(button.visualState() == GuiButton::VisualState::Pressed);

    button.onNotifyMouseEvent({MOUSE_MOVED_TO, cPoint(20, 25)});
    button.onNotifyMouseEvent({MOUSE_LEFT_BUTTON_CLICKED, cPoint(20, 25)});
    REQUIRE(clickCount == 1);
    REQUIRE(button.visualState() == GuiButton::VisualState::Hover);
}

TEST_CASE_METHOD(GuiButtonTestFixture, "GuiButton - disabled state blocks interaction and keeps the visual state disabled", "[gui]")
{
    GuiButton button(drawer.get(), cRectangle(10, 20, 100, 30), "Play");

    button.setEnabled(false);
    REQUIRE(button.isEnabled() == false);
    REQUIRE(button.visualState() == GuiButton::VisualState::Disabled);

    button.onNotifyMouseEvent({MOUSE_MOVED_TO, cPoint(20, 25)});
    REQUIRE(button.visualState() == GuiButton::VisualState::Disabled);

    button.onNotifyMouseEvent({MOUSE_MOVED_TO, cPoint(200, 25)});
    button.setEnabled(true);
    REQUIRE(button.isEnabled() == true);
    REQUIRE(button.visualState() == GuiButton::VisualState::Normal);
}

TEST_CASE_METHOD(GuiButtonTestFixture, "GuiButton - mouse click executes the click callback once when the pointer is inside the button", "[gui]")
{
    GuiButton button(drawer.get(), cRectangle(10, 20, 100, 30), "Play");
    int clickCount = 0;

    button.setOnLeftMouseButtonClickedAction([&]() {
        ++clickCount;
    });

    button.onNotifyMouseEvent({MOUSE_MOVED_TO, cPoint(20, 25)});
    button.onNotifyMouseEvent({MOUSE_LEFT_BUTTON_CLICKED, cPoint(20, 25)});

    REQUIRE(clickCount == 1);
    REQUIRE(button.visualState() == GuiButton::VisualState::Hover);
}
