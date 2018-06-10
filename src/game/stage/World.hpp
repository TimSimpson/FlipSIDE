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
//      This contains all of the "things" or entities floating around
//      this big old world of ours, called "the stage".
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
    // TODO: Actually, nope. Make this private!
    std::array<CharacterSprite, NUMSPRITES + 1> Sprite;

    // TODO: does this need to return an index?
    // TODO: maybe I can create a smart reference which, when a CharacterSprite
    //       dies, can stick around enough to tell people owning it "hey this
    //       thing is dead." I guess the pointee would need to know about it.
    //       It would be better than these accursed indices.
    CharacterSpriteRef find_closest_player(const CharacterSprite & enemy) const;

    // Returns a list of CharacterSprites of a certain kind.
    // TODO: seems like there should be a way to make a view only iterator
    //       over these...
    std::vector<std::reference_wrapper<const CharacterSprite>>
        find_by_kind(Kind k) const;

    // Gets coordinates roughly between all of one kind of thing
    // (this is for the camera when the kind is Player)
    boost::optional<glm::vec2> find_middle_of_group(Kind k) const;
};

}   }

#endif
