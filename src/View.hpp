#ifndef NND3D_VIEW_HPP
#define NND3D_VIEW_HPP
#pragma once

#include <lp3/gfx.hpp>
#include "World.hpp"

namespace nnd3d {

namespace core = lp3::core;
namespace gfx = lp3::gfx;

// Shows stuff
class View
{
public:
    View(core::MediaManager & media, World & world);

    void operator()(const glm::mat4 & previous);

	void DrawStuff(float fps);

	void ForceShowBackground();

    void LoadTexture(int which, const std::string & fileName, int howWide,
                     int howHigh);

	glm::ivec4 QBColor(int index);

    void UpdateSprites();
private:
	core::MediaManager & media;	
    std::unique_ptr<gfx::Texture> bgtexture;
    std::array<std::unique_ptr<gfx::Texture>, 10> AnimationTexture;
	gfx::programs::SimpleTextured program;
	gfx::Font font; // for fps
	gfx::ElementWriter<gfx::TexVert> font_elements;
	gfx::QuadArray<gfx::TexVert> font_quads;

	std::vector<gfx::ElementWriter<gfx::TexVert>> game_elements;

	World & world;
	std::array<int, 10> _texWidth;
	std::array<int, 10> _texHeight;
	std::array<Vertex, 4> bgverts;
	int bgWidth;
	int bgHeight;

	// Maps old vert array to a quad
	inline void draw_vert_to_quad(const Vertex * v, 
		                          gfx::Quad<gfx::TexVert> & quad) {
		quad.dl().set(v[0].x, v[0].y, v[0].z, v[0].tu, v[0].tv);
		quad.ul().set(v[1].x, v[1].y, v[1].z, v[1].tu, v[1].tv);
		quad.dr().set(v[2].x, v[2].y, v[2].z, v[2].tu, v[2].tv);
		quad.ur().set(v[3].x, v[3].y, v[3].z, v[3].tu, v[3].tv);
	}
	
	void draw_verts_as_quad(const Vertex * v, const int texIndex);

    gsl::owner<gfx::Texture *> load_image(const std::string & fileName);

	// This used to be an array. The problem is in the old code bgWidth is a 
	// different value, which leads to one off duplications EVERYWHERE.
	// So instead passing -1 here will return bgWidth.
	int texWidth(int index);

	// Similar notes on bgHeight and -1 applies as with texWidth above.
	int texHeight(int index);

};

}   // end namespace

#endif
