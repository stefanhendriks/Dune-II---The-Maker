
#include <SDL2/SDL.h>

class cTimeManager;

class cFocusManager {
public:
    cFocusManager(cTimeManager* timeManager);
    ~cFocusManager();

    void setActivateFocus(bool value);
    bool getActivateFocus() const;
    void onWindowsFocus(const SDL_WindowEvent& event);
    bool getFocus() const;
private:
    bool actifFocus = false;
    bool hasFocus = true;
    cTimeManager* m_timeManager = nullptr;
};