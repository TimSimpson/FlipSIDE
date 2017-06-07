#include "View.hpp"
#include "World.hpp"

namespace nnd3d {

namespace {
	constexpr int letters_max = 80 * 24;
	const glm::ivec2 res2d(World::FULLSCREENWIDTH, World::FULLSCREENHEIGHT);
}

View::View(core::MediaManager & media)
:	media(media),
	bgtexture(),
	AnimationTexture({}),
	program(),
	font{ media.load("Engine/apple_kid.fnt") },
	font_elements{ (letters_max * 4) },
	font_quads(font_elements.add_quads(letters_max)),
	game_elements{ World::NUMSPRITES * 4 },
	game_quads(game_elements.add_quads(World::NUMSPRITES))
{
}

void View::operator()(const glm::mat4 & previous) {	
	program.use();
	auto _2d = gfx::create_2d_screen(previous, res2d);
	program.set_mvp(_2d);
	program.set_texture(font.texture().gl_id());
	program.draw(font_elements);
}

void View::DrawStuff(float fps) {
	std::string s = str(boost::format("FPS: %d") % fps);
	gfx::write_string(font_quads, font, glm::vec2(520, 10), 0.0f, 40.0f, s);
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
