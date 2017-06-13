#include "Vb.hpp"

namespace nnd3d {

Vb::Vb(lp3::core::MediaManager & media)
:   media(media),
	rnd_dev(),
    rnd_gen(rnd_dev()),
    rnd_distribution(0.0, 1.0)
{}

std::fstream Vb::OpenForInput(const std::string & filePath) {
	std::fstream file;
	std::string fullPath = media.path(str(boost::format("FlipSIDE/%s")
		% filePath));
	LP3_LOG_DEBUG("Opening file %s...", fullPath);
	file.open(fullPath.c_str(), std::ios::in | std::ios::binary);
	if (file.bad() || file.fail())
	{
		LP3_LOG_ERROR("Error opening file! %s", filePath);
		LP3_THROW2(lp3::core::Exception, "Error opening file!");
	}
	return file;
}

double Vb::Rnd() {
    return rnd_distribution(rnd_gen);
}

}   // end ns
