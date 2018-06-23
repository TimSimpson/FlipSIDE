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

    using Sprite = SpriteLevelData;

    std::vector<TextureInfo> textures;
    std::vector<Sprite> sprites;
};


FSLevelFile load_fs_level(Vb & vb, const std::string & file);


}   }  // end namespace

#endif
