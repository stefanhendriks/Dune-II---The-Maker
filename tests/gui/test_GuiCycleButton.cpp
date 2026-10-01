#include <catch2/catch_test_macros.hpp>

#include <memory>

#include <SDL3/SDL.h>

#include "drawers/SDLDrawer.hpp"
#include "gui/GuiCycleButton.h"

namespace {

class GuiCycleButtonTestFixture {
public:
    GuiCycleButtonTestFixture()
    {
        REQUIRE(SDL_Init(SDL_INIT_VIDEO));

        window = SDL_CreateWindow("d2tm-gui-cyclebutton-tests", 128, 128, SDL_WINDOW_HIDDEN);
        REQUIRE(window != nullptr);

        renderer = SDL_CreateRenderer(window, nullptr);
        REQUIRE(renderer != nullptr);

        drawer = std::make_unique<SDLDrawer>(renderer);
    }

    ~GuiCycleButtonTestFixture()
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

TEST_CASE_METHOD(GuiCycleButtonTestFixture, "GuiCycleButton - nextValue and previousValue move through the list and notify the callback", "[gui][cycle_button]")
{
    GuiCycleButton button(drawer.get(), cRectangle(10, 10, 80, 20), {10, 20, 30});

    int lastValue = -1;
    button.setOnChanged([&lastValue](int value) {
        lastValue = value;
    });

    REQUIRE(button.getSelectedValue() == 10);
    button.nextValue();
    REQUIRE(button.getSelectedValue() == 20);
    REQUIRE(lastValue == 20);

    button.previousValue();
    REQUIRE(button.getSelectedValue() == 10);
    REQUIRE(lastValue == 10);
}

TEST_CASE_METHOD(GuiCycleButtonTestFixture, "GuiCycleButton - disabled button ignores value changes", "[gui][cycle_button]")
{
    GuiCycleButton button(drawer.get(), cRectangle(10, 10, 80, 20), {5, 10, 15});

    int callbackCount = 0;
    button.setOnChanged([&callbackCount](int) {
        ++callbackCount;
    });

    button.setEnabled(false);
    button.nextValue();
    button.previousValue();

    REQUIRE(button.isEnabled() == false);
    REQUIRE(button.getSelectedValue() == 5);
    REQUIRE(callbackCount == 0);
}
