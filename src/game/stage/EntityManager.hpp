#ifndef NND3D_EntityManager_HPP
#define NND3D_EntityManager_HPP
#pragma once

#include "World.hpp"
#include "../../Sound.hpp"

namespace nnd3d { namespace game {

// ----------------------------------------------------------------------------
// class EntityManager
// ----------------------------------------------------------------------------
//     Holds all the Sprites and offers semi-opaqure access to them.
//     (This replaces World1).
//     The goal is to change `CharacterSprite` into a class that represents
//     a physical entity, and then put most of the special logic into the
//     process class.
// ----------------------------------------------------------------------------
class EntityManager {
public:

    EntityManager(CharacterSprite & sprite);

    EntityManager(gsl::span<CharacterSprite> sprites_arg);

    CharacterSprite & grab_sprite();

    // Returns some number of sprites.
    gsl::span<CharacterSprite> grab_sprites(int count);

    // This only exists so I can make it compatable with legacy code.
    // In other words, it's a hack that should be removed ASAP.
    void skip_to(int new_index);

    // This is an even crappier hack! If I don't have it the cinematic at the
    // end of room one can't work.
    void go_back(int old_index);

    // Runs collision detection, calls back to handlers if things are touching
    // TODO: Having to pass sound and clock in here is ridiculous.
    //       This should change so that:
    //          1. whether or not to do anything when two objects touch each
    //             other is determined entirely by the "kind" property;
    //             each object would have a list for what kinds it needs to
    //             be notified about.
    //          2. The processes that are called back should handle playing
    //             sounds, flickering, etc.
    void run_collision_detection(Sound & sound, const double clock);
private:
    int s_index;
    gsl::span<CharacterSprite> sprites;
}; /// >


// ---------------------------------------------------------------------------
// class EntityManagerCO
// ---------------------------------------------------------------------------
//     This is a view of the entity manager that only allows creates.
// ---------------------------------------------------------------------------
class EntityManagerCO {
public:
    EntityManagerCO(EntityManager & manager);

    inline CharacterSprite & grab_sprite() {
        return manager.grab_sprite();
    }

    inline gsl::span<CharacterSprite> grab_sprites(int count) {
        return manager.grab_sprites(count);
    }
private:
    EntityManager & manager;
};
}   }

#endif
