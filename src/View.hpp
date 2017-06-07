#ifndef NND3D_VIEW_HPP
#define NND3D_VIEW_HPP
#pragma once

#include <lp3/gfx.hpp>

namespace nnd3d {

namespace core = lp3::core;
namespace gfx = lp3::gfx;

// Shows stuff
class View
{
public:
    View(core::MediaManager & media);

    void operator()(const glm::mat4 & previous);

	void DrawStuff(float fps);

    void LoadTexture(int which, const std::string & fileName, int howWide,
                     int howHigh);
	
private:
	core::MediaManager & media;
    std::unique_ptr<gfx::Texture> bgtexture;
    std::array<std::unique_ptr<gfx::Texture>, 10> AnimationTexture;
	gfx::programs::SimpleTextured program;
	gfx::Font font; // for fps
	gfx::ElementWriter<gfx::TexVert> font_elements;
	gfx::QuadArray<gfx::TexVert> font_quads;
	gfx::ElementWriter<gfx::TexVert> game_elements;
	gfx::QuadArray<gfx::TexVert> game_quads;

    gsl::owner<gfx::Texture *> load_image(const std::string & fileName);
};

}   // end namespace

#endif
