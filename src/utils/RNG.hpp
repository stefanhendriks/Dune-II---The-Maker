#pragma once
#include <random>

// Class that centralizes all requests for random numbers

class RNG {
public:
    //initialization of the number generator
    static std::mt19937& getGenerator();

    /**
     * Returns an integer between min (included) and max (included).
     * Ie, calling this function with params min 1, max 3 can return the numbers: 1,2,3
     * If you wish to get a random number until max (not including), use #getIntMaxExcl()
     * @param min (including)
     * @param max (including)
     * @return
     */
    static int genInt(int min, int max);

    /**
     * Returns an integer between min (included) and max (excluded).
     * Ie, calling this function with params min 1, max 3 can return the numbers: 1,2
     * If you wish to get a random number including max, use #getInt()
     * @param min (including)
     * @param maxExcl (excluding)
     * @return
     */
    static int genIntMaxExcl(int min, int maxExcl);

    //
    /**
     * Returns an integer between 0 (included) and max(excluded).
     * Effectively calling getIntMaxExcl(0, max)
     * @param min (including)
     * @param max (including)
     * @return
     */
    static int rnd(int max);
    // Returns a decimal number between min (included) and max (included)
    static double genDouble(double min, double max);
private:
    RNG() = default;
    RNG(const RNG&) = delete;
    RNG& operator=(const RNG&) = delete;
};