#ifndef NND3D_GAME_GAMESTATE_HPP
#define NND3D_GAME_GAMESTATE_HPP

namespace nnd3d { namespace game {

// ---------------------------------------------------------------------------
// class GameState
// ---------------------------------------------------------------------------
//     This is for all the persistant stuff that actually needs to stay
//     global for the course of a "game" (meaning from the time the game
//     starts ie after the title screen till the time it goes back to the
//     title screen).
//     This does not count things that either change or can be reinitialized
//     (or cached) between rooms, all of which will be managed elsewhere.
// ---------------------------------------------------------------------------
class GameState {
public:
    GameState();

    // TODO: Move all of the PlayerData stuff here

    inline int continues() const { return continue_count; }

    inline bool game_over() const { return continue_count < 0; }

    inline void use_continue() { --continue_count; }
private:
    int continue_count;
};

}   }

#endif
