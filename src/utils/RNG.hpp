#pragma once
#include <random>

// Class that centralizes all requests for random numbers

class RNG {
public:
    //initialization of the number generator
    static std::mt19937& getGenerator();
    //Returns an integer between min (included) and max (included)
    static int genInt(int min, int max);
    //Returns an integer between 0 (included) and max(excluded)
    static int rnd(int max);
    // Returns a decimal number between min (included) and max (included)
    static double genDouble(double min, double max);
private:
    RNG() = default;
    RNG(const RNG&) = delete;
    RNG& operator=(const RNG&) = delete;
};