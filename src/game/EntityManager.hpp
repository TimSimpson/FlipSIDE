#ifndef NND3D_EntityManager_HPP
#define NND3D_EntityManager_HPP
#pragma once

#include "World.hpp"

namespace nnd3d { namespace game {

/// EntityManager
/// ------
///     Holds all the Sprites and offers semi-opaqure access to them.
///     (This replaces World1).
/// <
class EntityManager {
public:
    EntityManager(World & world);

    // Returns some number of sprites.
    gsl::span<CharacterSprite> grab_sprites(int count);
private:
    int s_index;
	World & world;
}; /// >

}   }

#endif
