/**
 * @file cTimer.h
 *
 * Dune 2 - The Maker
 *
 * @author Stefan Hendriks & the D2TM Team
 * @www http://www.dune2themaker.com
 * @copyright Copyright (c) 2001 - 2026 D2TM Team
 * @license This software is released under the MIT License.
 *
 * Note: Dune 2 is a trademark of Westwood Studios/Electronic Arts.
 *
 * This is a non-commercial educational project.
 */

#pragma once



class cTimer {
public:
    explicit cTimer() : m_ticks(0) {}

    void increment() {
        m_ticks = m_ticks + 1;
    }

    void decrement() {
        m_ticks = m_ticks - 1;
    }

    bool incrementUntil(int value) {
        increment();
        if (m_ticks > value) {
            m_ticks = 0;
            return true;
        }
        return false;
    }

    bool decrementUntil(int value = 0) {
        if (m_ticks > value) {
            decrement();
            return true;
        }
        return false;
    }

    int get() const { 
        return m_ticks;
    }

    void reset(int value) {
        m_ticks = value;
    }

    void zero() {
        reset(0);
    }
private:
    int m_ticks;
};