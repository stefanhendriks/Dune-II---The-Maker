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
private:
    int m_ticks;
};