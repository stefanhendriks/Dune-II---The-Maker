#pragma once

#include <cstdio>
#include <string>
#include <map>
#include <chrono>

class cProfileMetric {
public:
    cProfileMetric();

    std::chrono::time_point<std::chrono::system_clock> lastRunMs;
    std::chrono::duration<double> totalRunMs;
    int sampled;
};

class cProfiler {
public:
    static cProfiler *getInstance();

    void reset();

    /**
     * Register metric since now
     * @param name
     */
    void start(std::string name);

    /**
     * Registers now, diffs with current metric. Accumulates results.
     * @param name
     */
    void sample(std::string name);

    void printResults();

private:
    cProfiler();

    ~cProfiler();

    cProfiler &operator=(const cProfiler &) = delete;

    cProfiler &operator=(cProfiler &&) = delete;

    cProfiler(const cProfiler &) = delete;

    cProfiler(cProfiler &&) = delete;

    std::FILE *m_file;

    std::map<std::string, cProfileMetric> metrics;
};
