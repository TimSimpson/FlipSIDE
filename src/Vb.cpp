#include "Vb.hpp"

namespace nnd3d {

Vb::Vb(lp3::core::MediaManager & media)
:   media(media),
	rnd_dev(),
    rnd_gen(rnd_dev()),
    rnd_distribution(0.0, 1.0)
{}

VbFile Vb::OpenForInput(const std::string & filePath) {
	const std::string full_path = media.path(filePath);
	VbFile file{full_path};
	return std::move(file);
}

double Vb::Rnd() {
    return rnd_distribution(rnd_gen);
}

}   // end ns
