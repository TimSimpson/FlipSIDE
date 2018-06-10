#ifndef NND3D_EntityManager_HPP
#define NND3D_EntityManager_HPP
#pragma once

#include "World.hpp"

namespace nnd3d { namespace game {

// ----------------------------------------------------------------------------
// class EntityManager
// ----------------------------------------------------------------------------
//     Holds all the Sprites and offers semi-opaqure access to them.
//     (This replaces World1).
// ----------------------------------------------------------------------------
class EntityManager {
public:
    EntityManager(World & world);

    CharacterSprite & grab_sprite();

    // Returns some number of sprites.
    gsl::span<CharacterSprite> grab_sprites(int count);

    // This only exists so I can make it compatable with legacy code.
    // In other words, it's a hack that should be removed ASAP.
    void skip_to(int new_index);

    // This is an even crappier hack! If I don't have it the cinematic at the
    // end of room one can't work.
    void go_back(int old_index);
private:
    int s_index;
    World & world;
}; /// >

}   }

#endif
