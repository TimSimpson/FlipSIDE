#include "Vb.hpp"

namespace nnd3d {

Vb::Vb(lp3::core::MediaManager & media_arg)
:   media(media_arg)
{
}

VbFile Vb::OpenForInput(const std::string & filePath) {
	const std::string full_path = media.path(filePath);
	VbFile file{full_path};
	return file;
}

}   // end ns
