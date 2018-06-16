#include "View.hpp"
#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>

#ifdef _MSC_VER
    // The old VB code constantly was tossing ints into floats and vice-versa,
    // and using lp3::narrow everywhere
    #pragma warning(disable: 4244)
#endif

namespace nnd3d { namespace view {

const glm::ivec2 View::resolution{ 640, 480 };

namespace {
    constexpr int letters_max = 80 * 24;
    const glm::vec4 normColor{ 1.0f, 1.0f, 1.0f, 1.0f };

    glm::vec2 find_texture_scale_factor(const glm::vec2 & original_size,
                                        const glm::vec2 current_size) {
        // The old game used textures with random dimensions. Emscripten though
        // vomits if it gets textures that aren't the same multiples of 2 in both
        // directions.

        // ... though let's make sure.
        LP3_ASSERT(current_size.x == current_size.y);

        // First, look at the "original size". The old game logic passed in the
        // size of every bitmap it used, and we can use that to know what the
        // used size of the texture was.

        // Next, find "desired size", which is the next multiple of 2 fit for
        // each texture.

        const auto get_desired_dimension = [](const float s) {
            for(float i = 2; i < 1024; i *= 2) {
                if (s <= i) {
                    return i;
                }
            }
            // Just ignore it.
            return s;
        };

        // The size will be the largest of either dimension.
        const float desired_max_dimension = std::max(
            get_desired_dimension(original_size.x),
            get_desired_dimension(original_size.y));

        // If the current size is less than the desired size, it means I scaled
        // the image down so Emscripten wouldn't run out of memory (let's call this
        // Lynx Ninja Gaiden III enhanced graphics mode).

        // However, if we use the desired size in the calculations, the
        // resulting tu and tv coordinates will work.

        //return desired_max_dimension / current_size.x;
        return glm::vec2{desired_max_dimension, desired_max_dimension};
    }

    int get_smart(StupidIndex index) {
        return index.value + 1;
    }
}

Vertex::Vertex()
:   x(0),
    y(0),
    z(0),
    rhw(0),
    color(1.0f, 1.0f, 1.0f, 1.0f),
    specular(0),
    tu(0),
    tv(0)
{
}


glm::vec4 qb_color(int index) {
    // If the code picks wrong, they get translucent yellow.
    switch(index) {
        case 0:
            return glm::vec4{0.0f, 0.0f, 0.0f, 1.0f};
        case 1:
            return glm::vec4{0.0f, 0.0f, 0.5f, 1.0f};
        case 2:
            return glm::vec4{0.0f, 0.5f, 0.0f, 1.0f};
        case 3:
            return glm::vec4{0.5f, 1.0f, 0.5f, 1.0f};
        case 4:
            return glm::vec4{0.5f, 0.0f, 0.0f, 1.0f};
        case 5:
            return glm::vec4{0.5f, 0.0f, 0.5f, 1.0f};
        case 6:
            return glm::vec4{0.5f, 0.5f, 0.0f, 1.0f};
        case 7:
            return glm::vec4{0.75f, 0.75f, 0.75f, 1.0f};
        case 8:
            return glm::vec4{0.25f, 0.25f, 0.25f, 1.0f};
        case 9:
            return glm::vec4{0.0f, 0.0f, 1.0f, 1.0f};
        case 10:
            return glm::vec4{0.0f, 1.0f, 0.0f, 1.0f};
        case 11:
            return glm::vec4{0.5f, 0.5f, 1.0f, 1.0f};
        case 12:
            return glm::vec4{1.0f, 0.0f, 0.0f, 1.0f};
        case 13:
            return glm::vec4{1.0f, 0.0f, 1.0f, 1.0f};
        case 14:
            return glm::vec4{1.0f, 1.0f, 0.0f, 1.0f};
        case 15:
            return glm::vec4{1.0f, 1.0f, 1.0f, 1.0f};
        default:
            LP3_ASSERT(false);  // Bad color selection.
            return glm::vec4{1.0f, 1.0f, 0.0f, 0.5f};
    }
}

Billboard::Billboard()
:   ul{},
    size{},
    z(0),
    tex_src_ul{},
    tex_src_dr{},
    texture_index(0),
    color{ 1, 1, 1, 1 },
    _visible(true),
    _flicker(false)
{
}

void Billboard::set(float ul_x, float ul_y, float width, float height) {
    this->ul.x = ul_x;
    this->ul.y = ul_y;
    this->size.x = width;
    this->size.y = height;
}

void Billboard::set(float ul_x, float ul_y, float width, float height,
                    float srcx1, float srcy1, float srcx2, float srcy2)
{
    this->ul.x = ul_x;
    this->ul.y = ul_y;
    this->size.x = width;
    this->size.y = height;
    this->tex_src_ul.x = srcx1;
    this->tex_src_ul.y = srcy1;
    this->tex_src_dr.x = srcx2;
    this->tex_src_dr.y = srcy2;
}

void Billboard::set_src(float srcx1, float srcy1, float srcx2, float srcy2)
{
    this->tex_src_ul.x = srcx1;
    this->tex_src_ul.y = srcy1;
    this->tex_src_dr.x = srcx2;
    this->tex_src_dr.y = srcy2;
}


void Billboard::set_visibility(Visibility value) {
    switch (value) {
        case Visibility::normal:
            this->_visible = true;
            this->_flicker = false;
            break;
        case Visibility::flicker:
            this->_flicker = true;
            break;
        case Visibility::invisible:
            this->_visible = false;
            this->_flicker = false;
            break;
        default:
            LP3_ASSERT(false);
    }
}

StupidIndex::StupidIndex()
:   value(-1)
{}

StupidIndex::StupidIndex(int _value)
:   value(_value)
{
    LP3_ASSERT(_value >= -1 && value < View::texture_count - 1);
}

View::View(core::MediaManager & media_arg, Vb & vb_arg)
:   disable_view(false),
    fps(0),
    history({}),
    media(media_arg),
    textures({}),
    program(),
    font{ media.load("apple_kid.fnt") },
    font_elements{ (letters_max * 4) },
    font_quads(font_elements.add_quads(letters_max)),
    game_elements(),
    vb(vb_arg),
    texture_sizes(),
    bgverts({})
{
    // Create one vertex buffer for the bgtexture
    game_elements.emplace_back(letters_max * 4);
    // Then others for each other texture. Make each one the maximum size it
    // might need to be to represent all sprites on the screen, even though
    // this is wasteful.
    const int NUMSPRITES = 150;  // 2018-06: yep this is dumb. FIX IT LATER.
    for (std::size_t i = 0; i < textures.size(); ++i) {
        game_elements.emplace_back(NUMSPRITES * 4);
    }
}

void View::operator()(const glm::mat4 & previous) {
    for (auto & ge : game_elements) {
        ge.reset();
    }

    std::string s = str(boost::format("FPS: %d") % fps);
    gfx::write_string(font_quads, font, glm::vec2(520, 10), 0.0f, 40.0f, s);

    for (auto & m_b : _billboards) {
        const auto & b = m_b;
        if (b.visible()) {
            gfx::Quad<gfx::TexCVert> quad
                = game_elements[b.texture_index].add_quad();
            gfx::upright_quad(quad, b.ul, (b.ul + b.size), b.z,
                b.tex_src_ul / texture_sizes[b.texture_index],
                b.tex_src_dr / texture_sizes[b.texture_index]);
            quad.dl().set_rgb(m_b.color);
            quad.dr().set_rgb(m_b.color);
            quad.ul().set_rgb(m_b.color);
            quad.ur().set_rgb(m_b.color);
        }
        m_b.run_effects();
    }

    // Now start calling OpenGL
    program.use();
    auto _2d = gfx::create_2d_screen(previous, resolution);
    program.set_mvp(_2d);

    for (int i = 0; i <= 10; ++i) {
        lp3::gfx::Texture * tex = textures[i].get();
        if (tex && 0 < game_elements[i].count()) {
            program.set_texture(tex->gl_id());
            program.draw(game_elements[i]);
        }
    }

    program.set_texture(font.texture().gl_id());
    program.draw(font_elements);
}

void View::LoadHistory(const TextureHistory & other_history)
{
    for (int i = 0; i < lp3::narrow<int>(other_history.size()); ++i) {
        const auto & call = other_history[i];
        if (call) {
            load_texture(i, call->fileName, call->size);
        }
    }
}

const View::TextureHistory & View::SaveHistory() {
    return history;
}


gsl::owner<gfx::Texture *> View::load_image(const std::string & fileName) {
    // Yes, this old game was so silly it used black as its color key. :/
    LP3_LOG_DEBUG("Loading texture %s", fileName);
    auto bmp_file = media.path(fileName);
    glm::ivec3 color_key[] = { glm::ivec3{ 0, 0, 0 }};
    return new gfx::Texture(
        IMG_Load(bmp_file.c_str()),
        gsl::make_span(color_key, 1));
}

void View::disable() {
    this->disable_view = true;
}

void View::enable() {
    this->disable_view = false;
    for (int i = 0; i < lp3::narrow<int>(history.size()); ++ i) {
        const auto & call = history[i];
        if (call) {
            load_texture(i, call->fileName, call->size);
        }
        history[i] = boost::none;
    }
}

void View::load_animation_file(std::array<view::AnimationFrame, 20> & frames,
                               const std::string & file) {
    auto f = vb.OpenForInput(file);
    std::string line;
    for (int j = 1; j <= 20; ++ j) {
        f.input(frames[j].x , frames[j].y,
                frames[j].x2, frames[j].y2);
        LP3_LOG_DEBUG("%d, %d", frames[j].x, frames[j].y);
        LP3_LOG_DEBUG("%d, %d", frames[j].x2, frames[j].y2);
        if (frames[j].x == -1) {
            //TSNOW: Unlike the original this will read into the other
            //       vars, even if the first one was -1.
            break;
        }
        frames[j].x --;
        frames[j].y --;
    }
}

void View::load_texture(int index, const std::string & fileName,
                        boost::optional<glm::ivec2> size)
{
    if (this->disable_view) {
        history[index] = LoadTextureCall{ fileName, size };
        return;
    }

    LP3_LOG_DEBUG("LoadTexture %i %s", index, fileName);

    textures[index].reset(load_image(fileName));
    if (boost::algorithm::ends_with(fileName, ".bmp") || !size) {
        texture_sizes[index] = textures[index]->size();
    } else {
        texture_sizes[index] = find_texture_scale_factor(
            size.get(),
            textures[index]->size());
    }
    LP3_LOG_DEBUG("   size = %i, %i",
                  texture_sizes[index].x, texture_sizes[index].y);
}


void View::LoadTexture(StupidIndex which, const std::string & fileName,
                       int howWide,
                       int howHigh) {
    load_texture(get_smart(which), fileName, glm::ivec2{ howWide, howHigh });
}

void View::report_fps(float _fps) {
    fps = _fps;
}



}   }   // end namespace
