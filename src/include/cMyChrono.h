#pragma once

#include <chrono>
#include <fstream>
#include <iomanip>
#include <mutex>
#include <string>

#include <iostream>

class MyChrono
{
public:
    using Clock = std::chrono::steady_clock;

    explicit MyChrono(std::string name)
        : m_name(std::move(name))
    {
        start();
    }

    ~MyChrono()
    {
        if (m_running)
        {
            stop();
        }
    }

    void start()
    {
        m_start = Clock::now();
        m_running = true;
    }

    void stop()
    {
        if (!m_running)
            return;

        m_running = false;

        const auto end = Clock::now();

        const double elapsedMs =
            std::chrono::duration<double, std::milli>(
                end - m_start).count();

        write(elapsedMs);
    }

private:
    static std::ofstream& file()
    {
        static std::ofstream ofs("perf_log.txt", std::ios::app);
        return ofs;
    }

    static std::mutex& mutex()
    {
        static std::mutex m;
        return m;
    }

    void write(double ms)
    {
        std::lock_guard<std::mutex> lock(mutex());

        if (ms<0.1)
            return; // ignore very fast calls

        auto& ofs = file();
        auto msg = std::format("{}: {:.3f} ms\n", m_name, ms);
        
        ofs << msg;

        std::cout << msg;
        // no flush here
    }

private:
    std::string m_name;
    Clock::time_point m_start;
    bool m_running = false;
};