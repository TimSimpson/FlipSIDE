#include "Vb.hpp"

namespace nnd3d {

Vb::Vb()
:   rnd_dev(),
    rnd_gen(rnd_dev()),
    rnd_distribution(0.0, 1.0)
{}

double Vb::Rnd() {
    return rnd_distribution(rnd_gen);
}

}   // end ns
