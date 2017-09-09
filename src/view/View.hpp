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


enum class Visibility {
	normal,
	invisible,
	flicker
};

struct Billboard {
	glm::vec2 ul;
	glm::vec2 size;
	float z;
	glm::vec2 tex_src_ul;
	glm::vec2 tex_src_dr;
	int texture_index;
	
	glm::vec4 color;

	void set_visibility(Visibility value);

	bool _visible;
	bool _flicker;

	Billboard();
};

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

    View(core::MediaManager & media, Vb & vb);

    void operator()(const glm::mat4 & previous);
	
	// These are manipulated directly by client code.
	std::vector<Billboard> & billboards() {
		return _billboards;
	}

    void LoadTexture(StupidIndex which, const std::string & fileName,
                     int howWide, int howHigh);

    // Fills up the frames of a character sprite with stuff from a file.
    void load_animation_file(std::array<view::AnimationFrame, 20> & frames,
                             const std::string & file);    

	// Turn off view
	void disable();

	void enable();

    // Tell the view what the FPS was
    void report_fps(float fps);
	
private:
	struct LoadTextureCall {
		std::string fileName;
		int howWide;
		int howHigh;
	};

	typedef std::array<boost::optional<LoadTextureCall>, texture_count>
		TextureHistory;


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

    Vb & vb;
	std::array<glm::vec2, texture_count> texture_sizes;
	std::array<Vertex, 4> bgverts;

	std::vector<Billboard> _billboards;

	void LoadHistory(const TextureHistory & history);

    gsl::owner<gfx::Texture *> load_image(const std::string & fileName);

	const TextureHistory &  SaveHistory();
};

}   }   // end namespace

#endif
