#ifndef NND3D_CONTROLS_HPP
#define NND3D_CONTROLS_HPP
#pragma once

#include <fstream>
#include <lp3/core.hpp>
#include <lp3/input.hpp>
#include <lp3/sims.hpp>
#include <string>

namespace nnd3d { namespace input {


enum class Key : int {
    up = 0,
    down = 1,
    left = 2,
    right = 3,
    attack = 4,
    jump = 5,
    pause = 6,
    quit = 7,
    skip_scene = 8,
    power_up = 9,
    lemon_time = 10,
};

struct Event {
    std::int8_t player;  // 0 for system events
    Key key;
    float value;
};


// TODO: Deprecated, remove soon.
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
    virtual std::vector<Event> retrieve_events(std::int64_t ms) = 0;
};


// --------------------------------------------------------------------
// InputMultiplexer
// --------------------------------------------------------------------
//      retrieves events from multiple InputProvider instances
// --------------------------------------------------------------------
class InputMultiplexer : public InputProvider {
public:
    InputMultiplexer();
    ~InputMultiplexer() override = default;

    void add_input(InputProvider * provider);

    std::vector<Event> retrieve_events(std::int64_t ms) override;
private:
    std::vector<InputProvider *> providers;
};

// --------------------------------------------------------------------
// LegacyInputProvider
// --------------------------------------------------------------------
//      Grabs events from the keyboard, translates to game input.
//      The keys used are based on the goofy layout of the original
//      VB game.
// --------------------------------------------------------------------
class LegacyInputProvider : public InputProvider {
public:
    LegacyInputProvider();
    ~LegacyInputProvider() override = default;

    void handle_events(const SDL_Event & event);

    std::vector<Event> retrieve_events(std::int64_t ms) override;
private:
    std::vector<Event> changes;
};

// --------------------------------------------------------------------
// ModernInputProvider
// --------------------------------------------------------------------
//      Reads from lp3::input Controls. It expects the instance passed
//      in to persist as long as this class does.
// --------------------------------------------------------------------
class ModernInputProvider : public InputProvider {
public:
    ModernInputProvider(lp3::input::Controls & controls);
    ~ModernInputProvider() override = default;

    std::vector<Event> retrieve_events(std::int64_t ms) override;
private:
    lp3::input::Controls & controls;
    std::array<lp3::input::ControlMemory, 3> memory;
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

    std::vector<Event> retrieve_events(std::int64_t ms) override;
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

    bool playback_finished() const;

    std::vector<Event> retrieve_events(std::int64_t ms) override;
private:
    lp3::sdl::RWops file;
    boost::optional<std::int64_t> next_time;
    std::int64_t time;

    boost::optional<Event> read_event();
};


}   }

#endif
