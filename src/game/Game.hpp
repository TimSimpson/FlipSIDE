#ifndef NND3D_GAME_HPP
#define NND3D_GAME_HPP
#pragma once

#include "../Input.hpp"
#include "../Random.hpp"
#include "../Sound.hpp"
#include "World.hpp"
#include "../Vb.hpp"
#include "../view.hpp"

namespace nnd3d { namespace game {

namespace core = lp3::core;
namespace gfx = lp3::gfx;

// The old code figured out the percentage of a second each frame
// took, and created a "speed factor" which it multiplied everything
// in existence by. That's actually a bad approach for several
// reasons, but the take away is here we introducing a constant
// speed mod which will always be 0.016
constexpr std::int64_t ms_per_update = 1000 / 60;  //16 ms for 60 fps

// So the game created a speed factor that tried to make it
// target 120fps (kind of cool my old Pentium 2 machine could do
// that). So we multiple the number we just had by 120.
constexpr float speed_factor = (ms_per_update / 1000.0f) * 120;

class GameProcess;

class GameProcessSpace {
public:
    GameProcessSpace();

	~GameProcessSpace();

    void exec(gsl::owner<GameProcess *> proc);

    inline GameProcess * get_proc() {
        return proc.get();
    }
private:
    std::unique_ptr<GameProcess> proc;
};


// Represents a unique screen or subroutine of the game, like the title
// screen, game over screen, etc.
class GameProcess {
public:
    virtual ~GameProcess() = default;

    // Drives the game. Think of this as the `input` to the game process.
    virtual void handle_input(const input::Event &) = 0;

    // As a rule, this is called every 16ms.
    // If this returns nullptr, the process continues.
    // If a new process is returned, this process is deleted and the new
    // process is used.
	virtual gsl::owner<GameProcess *> update() = 0;
};


// Subsystems the game processes typically need.
// This is the `output` of the game process.
struct GameContext {
	lp3::core::MediaManager & media;
	Sound & sound;
	view::View & view;
};

// Registers a game process by name, allowing it to be loaded via reflection.
class RegisterGameProcess {
public:
	RegisterGameProcess(const char * const name,
		                const char * const desc,
                        gsl::owner<GameProcess *>(*create)(GameContext context));
};

struct GameProcessEntry {
	const char * name;
	const char * desc;
	gsl::owner<GameProcess *>(*create)(GameContext context);
};

std::vector<GameProcessEntry> get_all_game_processes();

// Returns a game process. This makes the starting process changable from
// the command line.
boost::optional<GameProcessEntry> get_game_process_by_name(const char * name);

// --------------------------------------------------------------------
// Game
// --------------------------------------------------------------------
//      For now this is where the bulk of the code from NND3d2.bas
//      will live. The state from that mammoth module is represented
//      by the World struct.
// --------------------------------------------------------------------
class Game
{
public:
    Game(GameContext game_context, GameProcessEntry start_proc);

	~Game();

    void handle_input(const input::Event & even);

	bool quit() const;

    void update();

private:
    GameProcessSpace process;
	bool _quit;
};

}   }  // end namespace

#endif
