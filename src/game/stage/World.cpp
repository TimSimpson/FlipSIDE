#include "World.hpp"
#include <algorithm>

namespace nnd3d {   namespace game {

World::World()
:   camera(),
    Sprite()
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
    Sprite(other.Sprite)
{}
boost::optional<glm::vec2> World::find_middle_of_group(Kind k) const {
    bool found_any = false;
    glm::dvec2 min, max;
    for (const auto & s : Sprite) {
        if (s.kind == k) {
            if (!found_any) {
                found_any = true;
                min = s.getMiddle();
                max = s.getMiddle();
            } else {
                min.x = std::min(min.x, s.getMiddleX());
                min.y = std::min(min.y, s.getMiddleY());
                max.x = std::max(max.x, s.getMiddleX());
                max.y = std::max(max.y, s.getMiddleY());
            }
        }
    }
    if (found_any) {
        auto v = ((min + max) / 2.0);
		return glm::vec2{ v.x, v.y };
	} else {
		return boost::none;
	}
}

}   }   // end namespace
