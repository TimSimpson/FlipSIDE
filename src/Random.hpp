#ifndef NND3D_RANDOM_HPP
#define NND3D_RANDOM_HPP
#pragma once

#include <random>

namespace nnd3d {

// Wrapper over uniform_real_distribution. This exists to make it easier to
// swap out in the future.
// To make the game deterministic it's vital to always replay the same
// stream of random numbers.
class Random
{
public:
    Random();

    double next();
private:
    std::random_device rnd_dev;
    std::mt19937 rnd_gen;
    std::uniform_real_distribution<double> rnd_distribution;
};

}

#endif
