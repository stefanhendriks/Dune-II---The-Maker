#pragma once

#include <cstdio>
#include <string>
#include <map>
#include <chrono>

typedef unsigned long long timestamp_t;

class cProfileMetric {
public:
    cProfileMetric();

    bool finished;
    timestamp_t lastRunMs;
    double totalRunMs;
    int sampled;
};

class cProfiler {
public:
    cProfiler();
    ~cProfiler();

    void reset();

    /**
     * Register metric since now
     * @param name
     */
    void start(std::string name);

    void stop(std::string name);

    /**
     * Registers now, diffs with current metric. Accumulates results.
     * @param name
     */
    void sample(std::string name);

    void printResults();

private:

    cProfiler(const cProfiler &) = delete;

    cProfiler(cProfiler &&) = delete;

    std::FILE *m_file;

    std::map<std::string, cProfileMetric> metrics;
};
