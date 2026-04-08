#pragma once

#include <memory>

class cGameObjectContext;

class cGameObjectsContextCreator {
public:
    cGameObjectsContextCreator() = default;
    ~cGameObjectsContextCreator() = default;

    static std::unique_ptr<cGameObjectContext> create();
};
