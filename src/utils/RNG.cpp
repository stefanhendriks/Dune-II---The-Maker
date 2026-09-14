/**
 * @file RNG.cpp
 *
 * Dune 2 - The Maker
 *
 * @author Stefan Hendriks & the D2TM Team
 * @www http://www.dune2themaker.com
 * @copyright Copyright (c) 2001 - 2026 D2TM Team
 * @license This software is released under the MIT License.
 *
 * Note: Dune 2 is a trademark of Westwood Studios/Electronic Arts.
 *
 * This is a non-commercial educational project.
 */

#include "utils/RNG.hpp"

std::mt19937& RNG::getGenerator()
{
    static std::random_device rd;
    static std::mt19937 s_generator(rd());
    return s_generator;
}

int RNG::genInt(int min, int max)
{
    std::uniform_int_distribution<int> dist(min, max);
    return dist(getGenerator());
}

int RNG::genIntMaxExcl(int min, int maxExcl)
{
    std::uniform_int_distribution<int> dist(min, maxExcl-1);
    return dist(getGenerator());
}

int RNG::rnd(int max)
{
    if (max < 1) {
        return 0;
    }
    return genIntMaxExcl(0, max);
}

double RNG::genDouble(double min, double max)
{
    std::uniform_real_distribution<double> dist(min, max);
    return dist(getGenerator());
}