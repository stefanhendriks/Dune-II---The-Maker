#pragma once

struct s_KeysCombo {
    bool altPressed = false;
    bool ctrlPressed = false;
    bool shiftPressed = false;
};

enum class eKeyEventType {
    NONE,
    /**
     * key is being pressed down (hold)
     */
    HOLD,
    /**
     * key is held, then released. Makes a key "pressed".
     */
    PRESSED,
};