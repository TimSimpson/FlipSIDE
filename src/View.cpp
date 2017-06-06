#include "View.hpp"

namespace nnd3d {

View::View(core::MediaManager & media)
:   media(media),
	bgtexture(),
    AnimationTexture({})
{
}

void View::operator()(const glm::mat4 & previous) {
    // TODO: FILL IN.
}

gsl::owner<gfx::Texture *> View::load_image(const std::string & fileName) {
    // Yes, this old game was so silly it used black as its color key. :/
    LP3_LOG_DEBUG("Loading texture %s", fileName);
    std::string path = str(boost::format("FlipSIDE/%s") % fileName);
    auto bmp_file = media.path(path);
    glm::ivec3 color_key[] = { glm::ivec3{ 0, 0, 0 }};
    return new gfx::Texture(
        IMG_Load(bmp_file.c_str()),
        gsl::make_span(color_key, 1));
}

void View::LoadTexture(int which, const std::string & fileName, int howWide,
                       int howHigh) {
    if (which == -1) {
        bgtexture.reset(load_image(fileName));
    } else {
        AnimationTexture[which].reset(load_image(fileName));
    }
}

}   // end namespace
