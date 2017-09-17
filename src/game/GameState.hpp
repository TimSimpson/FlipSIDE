#ifndef NND3D_GAME_GAMESTATE_HPP
#define NND3D_GAME_GAMESTATE_HPP

namespace nnd3d { namespace game {

/// This is for stuff that truly needs to be global, that happens outside of
/// room or stage loading. Hopefully it stays small.
class GameState {
public:
    GameState();

    inline int continues() const { return continue_count; }

    inline bool game_over() const { return continue_count <= 0; }

    inline void use_continue() { --continue_count; }
private:
    int continue_count;
};

}   }

#endif
