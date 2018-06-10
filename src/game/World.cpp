#include "World.hpp"
#include <algorithm>

namespace nnd3d {   namespace game {

World::World()
:   camera(),
    Sprite(),
    game_state()
{

    std::fill(Sprite.begin(), Sprite.end(), CharacterSprite{});

    this->camera.set_size({640, 480});

    for (int j = 0; j < 100; ++j)
    {
        this->Sprite[j].visible = false;
    }
}

World::World(const World & other)
:   camera(other.camera),
    Sprite(other.Sprite),
    game_state(other.game_state)
{}

}   }   // end namespace
