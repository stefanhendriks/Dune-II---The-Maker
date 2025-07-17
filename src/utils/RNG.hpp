#pragma once
#include <random>

class RNG {
public:
    static std::mt19937& getGenerator() {
        static std::random_device rd;
        static std::mt19937 s_generator(rd());
        return s_generator;
    }

    static int genInt(int min, int max) {
        std::uniform_int_distribution<int> dist(min, max);
        return dist(getGenerator());
    }

    static int rnd(int max) {
        if (max<0)
            return 0;
        std::uniform_int_distribution<int> dist(0, max);
        return dist(getGenerator());
    }

    static double genDouble(double min, double max) {
        std::uniform_real_distribution<double> dist(min, max);
        return dist(getGenerator());
    }

private:
    RNG() = default;
    RNG(const RNG&) = delete;
    RNG& operator=(const RNG&) = delete;
};