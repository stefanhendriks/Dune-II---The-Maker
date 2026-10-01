#include <catch2/catch_test_macros.hpp>

#include <memory>

#include <SDL3/SDL.h>

#include "drawers/SDLDrawer.hpp"
#include "gui/GuiCheckBox.h"

namespace {

class GuiCheckBoxTestFixture {
public:
    GuiCheckBoxTestFixture()
    {
        REQUIRE(SDL_Init(SDL_INIT_VIDEO));

        window = SDL_CreateWindow("d2tm-gui-checkbox-tests", 128, 128, SDL_WINDOW_HIDDEN);
        REQUIRE(window != nullptr);

        renderer = SDL_CreateRenderer(window, nullptr);
        REQUIRE(renderer != nullptr);

        drawer = std::make_unique<SDLDrawer>(renderer);
    }

    ~GuiCheckBoxTestFixture()
    {
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

TEST_CASE_METHOD(GuiCheckBoxTestFixture, "GuiCheckBox - click toggles the checked state and fires the matching callback", "[gui][checkbox]")
{
    GuiCheckBox checkbox(drawer.get(), cRectangle(10, 10, 20, 20));

    bool checkedCallbackCalled = false;
    bool uncheckedCallbackCalled = false;
    checkbox.setCheckAction([&checkedCallbackCalled]() { checkedCallbackCalled = true; });
    checkbox.setUnCheckAction([&uncheckedCallbackCalled]() { uncheckedCallbackCalled = true; });

    checkbox.setChecked(false);
    checkbox.onNotifyMouseEvent({MOUSE_MOVED_TO, cPoint(15, 15)});
    checkbox.onNotifyMouseEvent({MOUSE_LEFT_BUTTON_CLICKED, cPoint(15, 15)});

    REQUIRE(checkbox.visualState() == GuiStatefulVisual::VisualState::Hover);
    REQUIRE(checkbox.isEnabled() == true);
    REQUIRE(checkedCallbackCalled == true);
    REQUIRE(uncheckedCallbackCalled == false);

    checkbox.onNotifyMouseEvent({MOUSE_LEFT_BUTTON_CLICKED, cPoint(15, 15)});

    REQUIRE(uncheckedCallbackCalled == true);
}

TEST_CASE_METHOD(GuiCheckBoxTestFixture, "GuiCheckBox - disabled state blocks toggling", "[gui][checkbox]")
{
    GuiCheckBox checkbox(drawer.get(), cRectangle(10, 10, 20, 20));
    checkbox.setChecked(false);
    checkbox.setEnabled(false);

    checkbox.onNotifyMouseEvent({MOUSE_MOVED_TO, cPoint(15, 15)});
    checkbox.onNotifyMouseEvent({MOUSE_LEFT_BUTTON_CLICKED, cPoint(15, 15)});

    REQUIRE(checkbox.isEnabled() == false);
    REQUIRE(checkbox.visualState() == GuiStatefulVisual::VisualState::Disabled);
}
