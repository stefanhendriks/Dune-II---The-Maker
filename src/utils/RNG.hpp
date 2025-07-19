#pragma once
#include <random>

class RNG {
public:
    static std::mt19937& getGenerator();

    static int genInt(int min, int max);

    static int rnd(int max);

    static double genDouble(double min, double max);

private:
    RNG() = default;
    RNG(const RNG&) = delete;
    RNG& operator=(const RNG&) = delete;
};