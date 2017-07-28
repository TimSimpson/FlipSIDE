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

	glm::ivec4 Rgb(int r, int g, int b) const;

	glm::vec4 QBColor(int index);

    void UpdateSprites();

    struct LoadTextureCall {
        std::string fileName;
        int howWide;
        int howHigh;
    };

    void LoadHistory(
        const std::vector<boost::optional<LoadTextureCall>> & history);

    const std::array<boost::optional<LoadTextureCall>, 11> &  SaveHistory();

private:

    std::array<boost::optional<LoadTextureCall>, 11> history;

	core::MediaManager & media;
    std::unique_ptr<gfx::Texture> bgtexture;
    std::array<std::unique_ptr<gfx::Texture>, 10> AnimationTexture;
	gfx::programs::ColoredTexture program;
	gfx::Font font; // for fps
	gfx::ElementWriter<gfx::TexCVert> font_elements;
	gfx::QuadArray<gfx::TexCVert> font_quads;

	std::vector<gfx::ElementWriter<gfx::TexCVert>> game_elements;

	World & world;
	std::array<glm::vec2, 10> tex_size;
	std::array<Vertex, 4> bgverts;
	glm::vec2 bg_size;

	// Maps old vert array to a quad
	inline void draw_vert_to_quad(const Vertex * v,
		                          gfx::Quad<gfx::TexCVert> & quad,
                                  float z) {
		quad.dl().set(v[0].x, v[0].y, z, v[0].tu, v[0].tv,
                      v[0].color.r, v[0].color.g, v[0].color.b, v[0].color.a);
		quad.ul().set(v[1].x, v[1].y, z, v[1].tu, v[1].tv,
                      v[1].color.r, v[1].color.g, v[1].color.b, v[1].color.a);
		quad.dr().set(v[2].x, v[2].y, z, v[2].tu, v[2].tv,
                      v[2].color.r, v[2].color.g, v[2].color.b, v[2].color.a);
		quad.ur().set(v[3].x, v[3].y, z, v[3].tu, v[3].tv,
                      v[3].color.r, v[3].color.g, v[3].color.b, v[3].color.a);
	}

	void draw_verts_as_quad(const Vertex * v, const int texIndex, float z);

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
