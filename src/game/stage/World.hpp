#ifndef NND3D_WORLD_HPP
#define NND3D_WORLD_HPP
#pragma once

#include <array>
#include "Camera.hpp"
#include "CharacterSprite.hpp"
#include "../PlayerData.hpp"

namespace nnd3d { namespace game {

// --------------------------------------------------------------------
// World
// --------------------------------------------------------------------
//      Contains the state of the "world", meaning the giant mess of
//      global variables the old code relied on.
// --------------------------------------------------------------------
struct World {
    World();

    World(const World & other);

    static constexpr int NUMSPRITES = 150;

    // TODO: this should also live in LegacyGame-
    //       and be initialized every time!
    Camera camera;

    // TODO: move this into LegacyGame -
    //       and let it initialize it every single time it's constructed! :O
    std::array<CharacterSprite, NUMSPRITES + 1> Sprite;
};

}   }

#endif
