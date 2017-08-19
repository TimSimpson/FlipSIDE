#include "Random.hpp"

namespace nnd3d {

Random::Random()
:   rnd_dev(),
    rnd_gen(rnd_dev()),
    rnd_distribution(0.0, 1.0)
{
    // Make the random numbers repeat each time we run the program.
    rnd_gen.seed(1);

    // If you check this out, it will be "1791095845"
    // the same as shown in the docs here:
    // http://en.cppreference.com/w/cpp/numeric/random/mersenne_twister_engine/seed
    //   LP3_LOG_INFO("after seed by 1: %d", rnd_gen());
}

double Random::next() {
    return rnd_distribution(rnd_gen);
}

}   // end ns
