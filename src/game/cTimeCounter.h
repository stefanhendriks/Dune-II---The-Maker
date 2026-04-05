#pragma once
#include <stdint.h>
#include <string>

class cTimeCounter {
public:
    cTimeCounter();
    ~cTimeCounter();

    void start();
    void pause();
    void restart();
    uint64_t getTime() const;
    void addTime(uint64_t delta);

private:
    uint64_t m_durationTimer = 0;
    bool m_isPartyTimer = false;
};