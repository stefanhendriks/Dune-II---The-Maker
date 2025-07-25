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

int RNG::rnd(int max)
{
    if (max<1)
        return 0;
    std::uniform_int_distribution<int> dist(0, max-1);
        return dist(getGenerator());
}

double RNG::genDouble(double min, double max)
{
    std::uniform_real_distribution<double> dist(min, max);
    return dist(getGenerator());
}