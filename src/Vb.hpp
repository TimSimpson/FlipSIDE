#ifndef NND3D_VB_HPP
#define NND3D_VB_HPP
#pragma once

#include <random>

namespace nnd3d {

// Helper class which simulates old Visual Basic functions.
class Vb
{
public:
    Vb();

    double Rnd();
private:
    std::random_device rnd_dev;
    std::mt19937 rnd_gen;
    std::uniform_real_distribution<double> rnd_distribution;
};

}

#endif
