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

CharacterSpriteRef World::find_closest_player(const CharacterSprite & bad_guy)
const
{
    //numberPlayers integer legend
    //1 Only player 1
    //2 Player 1 and 2
    //3 All three Players
    //4 Just player 2
    //5 Just player 3
    //6 Players 1 and 3
    //7 Players 2 and 3

    int theclosest = 0;
    int min = 0;

    min = 9999;
    theclosest = 0;
    for (std::size_t index = 0; index < this->Sprite.size(); ++ index) {
        auto & player_sprite = this->Sprite[index];
        if (player_sprite.kind != Kind::player) {
            continue;
        }

        // abs(x2-x1)^2+abs(y2-y1)^2
        const int buttcat = lp3::narrow<int>(
            std::sqrt(
                std::pow(
                    std::abs(player_sprite.x - bad_guy.x),
                    2)
                +
                std::pow(
                    std::abs(player_sprite.y - bad_guy.y),
                    2)
            ));

        if (buttcat < min) {
            theclosest = index;
            min = buttcat;
        }
    }

    CharacterSpriteRef ref;
    ref.set(this->Sprite[theclosest]);
    return ref;
}

std::vector<std::reference_wrapper<const CharacterSprite>>
    World::find_by_kind(Kind k) const
{
    std::vector<std::reference_wrapper<const CharacterSprite>> result;
    for (const auto & s : Sprite) {
        if (s.kind == k) {
            result.push_back(std::cref(s));
        }
    }
    return result;
}
}   }   // end namespace
