#include "EntityManager.hpp"

namespace nnd3d { namespace game {

EntityManager::EntityManager(World & world) 
:	s_index(0),
	world(world) 
{}

gsl::span<CharacterSprite> EntityManager::grab_sprites(int count) {
    LP3_ASSERT(count > 0);
    LP3_ASSERT(count < world.Sprite.size());
	auto old_index = s_index;
	s_index += count;
    return gsl::make_span(&(world.Sprite[old_index]), count);
}

}   }
