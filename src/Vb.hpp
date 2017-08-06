#ifndef NND3D_VB_HPP
#define NND3D_VB_HPP
#pragma once

#include <fstream>
#include <random>
#include <lp3/core.hpp>
#include "VbFile.hpp"

namespace nnd3d {

// Helper class which simulates old Visual Basic functions.
class Vb
{
public:
    Vb(lp3::core::MediaManager & media);

	VbFile && OpenForInput(const std::string & filePath);

    double Rnd();
private:
	lp3::core::MediaManager & media;
    std::random_device rnd_dev;
    std::mt19937 rnd_gen;
    std::uniform_real_distribution<double> rnd_distribution;
};

}

#endif
