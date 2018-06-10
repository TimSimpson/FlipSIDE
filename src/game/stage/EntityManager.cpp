#include "EntityManager.hpp"

namespace nnd3d { namespace game {

EntityManager::EntityManager(World & world_arg)
:   s_index(0),
    world(world_arg)
{}

CharacterSprite & EntityManager::grab_sprite() {
    LP3_ASSERT(lp3::narrow<std::size_t>(s_index) < world.Sprite.size());
    auto old_index = s_index;
    s_index += 1;
    return world.Sprite[old_index];
}

gsl::span<CharacterSprite> EntityManager::grab_sprites(int count) {
    LP3_ASSERT(count > 0);
    LP3_ASSERT(lp3::narrow<std::size_t>(s_index + count) < world.Sprite.size());
    auto old_index = s_index;
    s_index += count;
    return gsl::make_span(&(world.Sprite[old_index]), count);
}

void EntityManager::skip_to(int new_index) {
    LP3_ASSERT(s_index < new_index);
    s_index = new_index;
}

void EntityManager::go_back(int new_index) {
    s_index = new_index;
}

}   }
