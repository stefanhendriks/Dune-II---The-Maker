#include "cProfiler.h"

#include "fmt/format.h"
#include "player/cPlayer.h"
#include "d2tmc.h"
#include <cstdio>
#include <chrono>
#include <system_error>
#include <sys/time.h>


static timestamp_t get_timestamp () {
//    struct timeval now;
//    timeval now;
//    gettimeofday (&now, nullptr);
//    return now.tv_usec + (timestamp_t)now.tv_sec * 1000;

    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

cProfileMetric::cProfileMetric() {
    finished = false;
    lastRunMs = get_timestamp();
    totalRunMs = 0;
    sampled = 0;
}

cProfiler::cProfiler() : m_file(std::fopen("profiler.txt", "a+")) {
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
    metrics[name].lastRunMs = get_timestamp();
}

void cProfiler::stop(std::string name) {
    sample(name);
    metrics[name].finished = true;
}

void cProfiler::printResults() {
    fprintf(m_file, "Results:\n");
//    players[0].clearNotifications();
    for (auto &pair: metrics) {
        cProfileMetric &metric = pair.second;
        sample(pair.first); // last sample when printing

        std::string logline = fmt::format("Metric {} took {} ms ({} sampled)", pair.first, metric.totalRunMs, metric.sampled);
        fprintf(m_file, "%s\n", logline.c_str()); // print the text into the file

        players[0].addNotification(logline, eNotificationType::DEBUG);
    }
    fflush(m_file);
}

void cProfiler::sample(std::string name) {
    timestamp_t now = get_timestamp();

    cProfileMetric *metric = &metrics[name];
    if (metric->finished) {
        return; // don't do anything when finished
    }

//    double t_elapsed = (t1 - t0) / 1000000.0L / 3;
    double timePassed = (now - metric->lastRunMs);
    metric->lastRunMs = now;

    metric->totalRunMs += (timePassed);
    metric->sampled++;
}