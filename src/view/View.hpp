#ifndef NND3D_VIEW_VIEW_HPP
#define NND3D_VIEW_VIEW_HPP
#pragma once

#include <lp3/gfx.hpp>
#include "../game/World.hpp"
#include "../Vb.hpp"

namespace nnd3d { namespace view {

namespace core = lp3::core;
namespace gfx = lp3::gfx;
namespace game = nnd3d::game;


glm::vec4 qb_color(int index);

// An index from the old code, where -1 meant the bg texture
// This has an assertion that the index is in bounds. Also using it in the
// method signatures lets me see what has yet to be updated.
struct StupidIndex {
    StupidIndex(int);
    int value;
};

// Shows stuff
class View
{
public:
    static constexpr int texture_count = 11;

    View(core::MediaManager & media, game::World & world, Vb & vb);

    void operator()(const glm::mat4 & previous);

	// Call this to update periodic animations
	void animate();

    void LoadTexture(StupidIndex which, const std::string & fileName,
                     int howWide, int howHigh);

    // Fills up the frames of a character sprite with stuff from a file.
    void load_animation_file(std::array<view::AnimationFrame, 20> & frames,
                             const std::string & file);

    struct LoadTextureCall {
        std::string fileName;
        int howWide;
        int howHigh;
    };

	typedef std::array<boost::optional<LoadTextureCall>, texture_count>
         TextureHistory;

	// TODO: Called by Main Loop - move this somewhere Game doesn't need to touch it.
	// Turn off view
	void disable();

	void enable();

	// TODO: Called by Main Loop - move this somewhere Game doesn't need to touch it.
    void LoadHistory(const TextureHistory & history);

    // Tell the view what the FPS was
    void report_fps(float fps);

	// TODO: Called by Main Loop - move this somewhere Game doesn't need to touch it.
    const TextureHistory &  SaveHistory();

private:
	bool disable_view;

    float fps;

	TextureHistory history;

	core::MediaManager & media;
    std::array<std::unique_ptr<gfx::Texture>, texture_count> textures;
	gfx::programs::ColoredTexture program;
	gfx::Font font; // for fps
	gfx::ElementWriter<gfx::TexCVert> font_elements;
	gfx::QuadArray<gfx::TexCVert> font_quads;

	std::vector<gfx::ElementWriter<gfx::TexCVert>> game_elements;

	game::World & world;
    game::Camera camera;
    Vb & vb;
	std::array<glm::vec2, texture_count> texture_sizes;
	std::array<Vertex, 4> bgverts;

    void DrawStuff();

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

	void draw_verts_as_quad(const Vertex * v, const StupidIndex texIndex, float z);

    gsl::owner<gfx::Texture *> load_image(const std::string & fileName);

    void UpdateSprites();
};

}   }   // end namespace

#endif
