#include "cProfiler.h"

#include "fmt/format.h"
#include "player/cPlayer.h"
#include "d2tmc.h"
#include <cstdio>
#include <chrono>
#include <system_error>

cProfileMetric::cProfileMetric() {
    lastRunMs = std::chrono::system_clock::now();
    sampled = 0;
}

cProfiler *cProfiler::getInstance() {
    // The logger is initialized once as soon as this function is called for the first time.
    static cProfiler theProfiler;
    return &theProfiler;
}

cProfiler::cProfiler() : m_file(std::fopen("profiler.txt", "wt")) {
    if (m_file == nullptr) {
        // This translates the POSIX error number into a C++ exception
        throw std::system_error(errno, std::generic_category());
    }

    metrics = std::map<std::string, cProfileMetric>();
    metrics.clear();
}

cProfiler::~cProfiler() {
    std::fclose(m_file);
}

void cProfiler::reset() {
    metrics.clear();
}

void cProfiler::start(std::string name) {
    if (metrics.count(name) == 0) {
        metrics[name] = cProfileMetric();
    }
    // (re)start for the next sample
    metrics[name].lastRunMs = std::chrono::system_clock::now();
}

void cProfiler::printResults() {
    fprintf(m_file, "Results:\n");
    players[0].clearNotifications();
    for (auto &pair: metrics) {
        cProfileMetric &metric = pair.second;
        auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(metric.totalRunMs).count();
        std::string logline = fmt::format("Metric {} took {} ms ({} sampled)", pair.first, millis, metric.sampled);
        fprintf(m_file, "%s\n", logline.c_str()); // print the text into the file

        players[0].addNotification(logline, eNotificationType::DEBUG);
    }
    fflush(m_file);
}

void cProfiler::sample(std::string name) {
    auto now = std::chrono::system_clock::now();
    cProfileMetric *metric = &metrics[name];
    std::chrono::duration<double> diff = now - metric->lastRunMs;
    metric->lastRunMs = now;
    metric->totalRunMs += diff;
    metric->sampled++;
}