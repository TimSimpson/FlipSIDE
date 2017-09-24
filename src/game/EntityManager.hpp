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

	CharacterSprite & grab_sprite();

    // Returns some number of sprites.
    gsl::span<CharacterSprite> grab_sprites(int count);

	// This only exists so I can make it compatable with legacy code.
	// In other words, it's a hack that should be removed ASAP.
	void skip_to(int new_index);
private:
    int s_index;
	World & world;
}; /// >

}   }

#endif
