// ---------------------------------------------------------------------------
// Procs
// =====
//      A model of a process where many may run in each room.
// ---------------------------------------------------------------------------
#ifndef NND3D_CHARACTERPROC_HPP
#define NND3D_CHARACTERPROC_HPP
#pragma once

#include "CharacterSprite.hpp"
#include "EntityManager.hpp"
#include "../Game.hpp"
#include "../../Random.hpp"
#include "../../Sound.hpp"
#include "../../view.hpp"

namespace nnd3d { namespace game {

class CharacterSprite;
class CharacterProc;
class CharacterProcInterop;
class CharacterProcManager;




// ---------------------------------------------------------------------------
// class CharacterProcInterop
// ---------------------------------------------------------------------------
//     This is a subset of functionality from CharacterProcManager that
//     allows the procs themselves to do things like launch other processes.
// ---------------------------------------------------------------------------
class CharacterProcInterop {
public:
    CharacterProcInterop(CharacterProcManager & manager);

    // Creates a new process.
    void spawn(gsl::owner<CharacterProc *> process);
private:
    CharacterProcManager & manager;
};


// ---------------------------------------------------------------------------
// class CharacterProcEnv
// ---------------------------------------------------------------------------
//      Stuff that every proc might need (holy cow that's a lot).
// ---------------------------------------------------------------------------
struct CharacterProcEnv {
    GameContext context;
    Random & random;
    const double & current_time;
    Camera & camera;
    CharacterProcInterop processes;
};

////void create_player(PlayerData & player_data, CharacterSprite & sprite,
////                   gsl::span<std::reference_wrapper<CharacterSprite>> & children,
////                   CharacterProcEnv env);


// ---------------------------------------------------------------------------
// class CharacterProc
// ---------------------------------------------------------------------------
//      This attempts to decouple the logic associated with sprites.
//      The ideas is multiple procs can run for each room.
//      Currently it's a bit of a mess.
// ---------------------------------------------------------------------------
class CharacterProc {
public:
     virtual ~CharacterProc() = default;

     // TODO: The plethora of virtual methods with default impls below was to
     // support the legacy procs, which needed a gross, large interface.
     // In the future delete them.
     virtual void animate(std::int64_t) {
     }

     // This should be changed to something that register a hit was detected.
     virtual void death_animation() {
         throw std::logic_error("The method or operation is not implemented.");
     }

     virtual bool update() = 0;
};


class InputReceivingCharacterProc : public CharacterProc {
public:
    virtual ~InputReceivingCharacterProc() = default;
    virtual void handle_input(const input::Event & event) = 0;
};

// void load_process(CharacterProcEnv env, CharacterSprite & s, const std::string &);

// void create_player(
//     CharacterProcEnv env, PlayerData & player_data, CharacterSprite & sprite,
//     gsl::span<CharacterSprite> & children);


// ---------------------------------------------------------------------------
// class CharacterProcManager
// ---------------------------------------------------------------------------
//     Manages the processes.
//     Note: unlike the sprite list, this isn't tied directly to indices or
//     anything like that.
// ---------------------------------------------------------------------------
class CharacterProcManager {
public:
    CharacterProcManager();

    ~CharacterProcManager();

    CharacterProcManager(CharacterProcManager && rhs);

    void add_process(gsl::owner<CharacterProc *> process);

    void animate(std::int64_t ms);

    void clear();

    void update();
private:
    // TODO: why isn't this std::unique_ptr?
    std::vector<gsl::owner<CharacterProc *>> procs;
};

}    }

#endif
