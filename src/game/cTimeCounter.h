/**
 * @file cTimeCounter.h
 *
 * Dune 2 - The Maker
 *
 * @author Stefan Hendriks & the D2TM Team
 * @www http://www.dune2themaker.com
 * @copyright Copyright (c) 2002 - 2026 Stefan Hendriks and contributors
 * @license This software is released under the MIT License. See LICENSE.md.
 *
 * Note: Dune 2 is a trademark of Westwood Studios/Electronic Arts.
 *
 * This is a non-commercial educational project.
 */

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