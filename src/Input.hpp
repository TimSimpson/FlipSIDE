#ifndef NND3D_CONTROLS_HPP
#define NND3D_CONTROLS_HPP
#pragma once

#include <fstream>
#include <lp3/core.hpp>
#include <string>

namespace nnd3d { namespace input {

struct StateChange {
    bool on;
    std::string key_name;
};

// --------------------------------------------------------------------
// InputProvider
// --------------------------------------------------------------------
//      General interface for all game input, used by main loop
// --------------------------------------------------------------------
class InputProvider {
public:
    virtual ~InputProvider() = default;

    // Called by when the loop wants to retrieve new events.
    virtual std::vector<StateChange> retrieve_events(std::int64_t ms) = 0;
};

// --------------------------------------------------------------------
// KeyboardInputProvider
// --------------------------------------------------------------------
//      Grabs events from the keyboard, translates to game input.
// --------------------------------------------------------------------
class KeyboardInputProvider : public InputProvider {
public:
	KeyboardInputProvider();
    ~KeyboardInputProvider() override = default;

    void handle_events(const SDL_Event & event);

    std::vector<StateChange> retrieve_events(std::int64_t ms) override;
private:
	std::vector<StateChange> changes;
};

// --------------------------------------------------------------------
// InputRecorder
// --------------------------------------------------------------------
//      Passes events to another InputProvider instance, recording all
//      events.
// --------------------------------------------------------------------
class InputRecorder : public InputProvider {
public:
    InputRecorder(lp3::sdl::RWops && write_file, InputProvider & input);
    ~InputRecorder() override = default;

    std::vector<StateChange> retrieve_events(std::int64_t ms) override;
private:
	lp3::sdl::RWops file;
	InputProvider & provider;
	std::int64_t time;
};


// --------------------------------------------------------------------
// InputPlayback
// --------------------------------------------------------------------
//      Plays back previously recorded input from a file.
// --------------------------------------------------------------------
class InputPlayback : public InputProvider {
public:
    ~InputPlayback() override = default;

    InputPlayback(lp3::sdl::RWops && read_file);
    std::vector<StateChange> retrieve_events(std::int64_t ms) override;
private:
    lp3::sdl::RWops file;
	boost::optional<std::int64_t> next_time;
	std::int64_t time;
	
	StateChange read_event();
};


}   }

#endif
