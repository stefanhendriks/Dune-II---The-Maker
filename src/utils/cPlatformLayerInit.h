#pragma once

#include <string>

// Initialization object for the platform abstraction layer (Allegro, SDL, etc.) that
// provides services for drawing on the screen, playing sound, handling input, etc.
// Initializes the layer in the constructor and de-initializes it in the destructor.
// Create only once and keep the object alive for the duration of the game.
class cPlatformLayerInit {
public:
    cPlatformLayerInit(const std::string &path_to_config_file, const std::string &window_title);
    ~cPlatformLayerInit();

    cPlatformLayerInit &operator=(const cPlatformLayerInit &) = delete;
    cPlatformLayerInit &operator=(cPlatformLayerInit &&) = delete;
    cPlatformLayerInit(const cPlatformLayerInit &) = delete;
    cPlatformLayerInit(cPlatformLayerInit &&) = delete;
};