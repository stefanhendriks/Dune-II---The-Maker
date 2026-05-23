#pragma once

#include "include/sGameEvent.h"

#include <functional>
#include <utility>

class cEventEmitter {
public:
    using DispatchFn = std::function<void(const s_GameEvent &)>;

    explicit cEventEmitter(DispatchFn dispatchFn)
        : m_dispatchFn(std::move(dispatchFn)) {
    }

    void emit(const s_GameEvent &event) const {
        if (m_dispatchFn) {
            m_dispatchFn(event);
        }
    }

private:
    DispatchFn m_dispatchFn;
};
