#include <catch2/catch_test_macros.hpp>

#include <memory>

#include <SDL3/SDL.h>

#include "drawers/SDLDrawer.hpp"
#include "gui/GuiSlider.h"

namespace {

class GuiSliderTestFixture {
public:
    GuiSliderTestFixture()
    {
        REQUIRE(SDL_Init(SDL_INIT_VIDEO));

        window = SDL_CreateWindow("d2tm-gui-slider-tests", 128, 128, SDL_WINDOW_HIDDEN);
        REQUIRE(window != nullptr);

        renderer = SDL_CreateRenderer(window, nullptr);
        REQUIRE(renderer != nullptr);

        drawer = std::make_unique<SDLDrawer>(renderer);
    }

    ~GuiSliderTestFixture()
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

TEST_CASE_METHOD(GuiSliderTestFixture, "GuiSlider - mouse drag updates the current value and fires the callback", "[gui][slider]")
{
    GuiSlider slider(drawer.get(), cRectangle(10, 10, 100, 12), 0, 100, 0);

    int lastValue = -1;
    slider.setOnValueChanged([&lastValue](int value) {
        lastValue = value;
    });

    slider.onNotifyMouseEvent({MOUSE_LEFT_BUTTON_PRESSED, cPoint(60, 15)});
    REQUIRE(slider.getValue() == 50);
    REQUIRE(lastValue == 50);

    slider.onNotifyMouseEvent({MOUSE_MOVED_TO, cPoint(80, 15)});
    REQUIRE(slider.getValue() == 70);
    REQUIRE(lastValue == 70);
}

TEST_CASE_METHOD(GuiSliderTestFixture, "GuiSlider - disabled slider ignores pointer input", "[gui][slider]")
{
    GuiSlider slider(drawer.get(), cRectangle(10, 10, 100, 12), 0, 100, 20);

    int callbackCount = 0;
    slider.setOnValueChanged([&callbackCount](int) {
        ++callbackCount;
    });

    slider.setEnabled(false);
    slider.onNotifyMouseEvent({MOUSE_LEFT_BUTTON_PRESSED, cPoint(80, 15)});
    slider.onNotifyMouseEvent({MOUSE_MOVED_TO, cPoint(90, 15)});

    REQUIRE(slider.isEnabled() == false);
    REQUIRE(slider.getValue() == 20);
    REQUIRE(callbackCount == 0);
}
