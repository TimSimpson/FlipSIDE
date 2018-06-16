#ifndef NND3D_LEVEL_HPP
#define NND3D_LEVEL_HPP
#pragma once

#include "CharacterSprite.hpp"
#include "../../view/View.hpp"

namespace nnd3d { namespace game {


// Represents an old school FlipSide level file
struct FSLevelFile {
    struct TextureInfo {
        int index;
        std::string name;
        glm::ivec2 size;
    };

    struct Sprite {
        std::string name;
        glm::dvec3 position;
        glm::ivec2 src_ul;
        glm::ivec2 src_br;
        glm::dvec3 size;
        view::StupidIndex texture;
        bool visible;
        Kind kind;
        int z_order;
    };

    std::vector<TextureInfo> textures;
    std::vector<Sprite> sprites;
};


FSLevelFile load_fs_level(Vb & vb, const std::string & file);


}   }  // end namespace

#endif
