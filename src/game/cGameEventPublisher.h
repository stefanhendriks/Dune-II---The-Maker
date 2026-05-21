#pragma once

#include "include/sGameEvent.h"

#include <functional>
#include <utility>

class cGameEventPublisher {
public:
    using DispatchFn = std::function<void(const s_GameEvent &)>;

    explicit cGameEventPublisher(DispatchFn dispatchFn)
        : m_dispatchFn(std::move(dispatchFn)) {
    }

    void emit(const s_GameEvent &event) const {
            m_dispatchFn(event);
    }

private:
    DispatchFn m_dispatchFn;
};
