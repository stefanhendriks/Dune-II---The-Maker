#pragma once

#include <memory>

class cGameObjectContext;
struct sGameServices;

class cGameObjectsContextCreator {
public:
    cGameObjectsContextCreator() = default;
    ~cGameObjectsContextCreator() = default;

    static std::unique_ptr<cGameObjectContext> create(sGameServices* services);
};
