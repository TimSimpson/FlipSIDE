#include "Input.hpp"

namespace nnd3d { namespace input {

namespace {
    std::string key_name_from_sdl(const SDL_Keycode & kc) {
        // Maps the SDL keyboard names back to what they were in Visual Basic 6.
        switch (kc) {
        case SDLK_ESCAPE: return "escape";
        case SDLK_UP: return "up";
        case SDLK_DOWN: return "down";
        case SDLK_LEFT: return "left";
        case SDLK_RIGHT: return "right";
        default:
            // The letters from SDL s/b upper case. Anything else from SDL we
            // can safely return as it will just be ignored.
            std::string name(SDL_GetKeyName(kc));
            return name;
        }
    }

    struct LegacyKeyToNewKey {
        const char * old_name;
        std::int8_t player;
        Key key;
    };

    const LegacyKeyToNewKey legacy_keys_to_new_keys[] = {
        LegacyKeyToNewKey{ "W", 0, Key::up },
        LegacyKeyToNewKey{ "S", 0, Key::down },
        LegacyKeyToNewKey{ "A", 0, Key::left },
        LegacyKeyToNewKey{ "D", 0, Key::right },
        LegacyKeyToNewKey{ "F", 0, Key::attack },
        LegacyKeyToNewKey{ "G", 0, Key::jump },
        LegacyKeyToNewKey{ "up", 1, Key::up },
        LegacyKeyToNewKey{ "down", 1, Key::down },
        LegacyKeyToNewKey{ "left", 1, Key::left },
        LegacyKeyToNewKey{ "right", 1, Key::right },
        LegacyKeyToNewKey{ "O", 1, Key::attack },
        LegacyKeyToNewKey{ "P", 1, Key::jump },
        LegacyKeyToNewKey{ "I", 2, Key::up },
        LegacyKeyToNewKey{ "K", 2, Key::down },
        LegacyKeyToNewKey{ "J", 2, Key::left },
        LegacyKeyToNewKey{ "L", 2, Key::right },
        LegacyKeyToNewKey{ "Y", 2, Key::attack },
        LegacyKeyToNewKey{ "U", 2, Key::jump },
        LegacyKeyToNewKey{ "escape", -1, Key::quit },
        LegacyKeyToNewKey{ "T", -1, Key::skip_scene },
        LegacyKeyToNewKey{ "R", -1, Key::power_up },
        LegacyKeyToNewKey{ "Y", -1, Key::lemon_time },
        LegacyKeyToNewKey{ nullptr, -1, Key::quit }
    };

    boost::optional<Event> state_change_to_event(const StateChange & sc) {
        for (const LegacyKeyToNewKey * itr = &legacy_keys_to_new_keys[0];
             nullptr != itr->old_name; ++ itr )
        {
            const LegacyKeyToNewKey & mapping = *itr;
            if (sc.key_name == mapping.old_name) {
                return Event {
                    mapping.player,
                    mapping.key,
                    sc.on ? 1.0f : 0.0f
                };
            }
        }
        return boost::none;
    }

    StateChange event_to_legacy_state(const Event & ev) {
        for (std::size_t i = 0; i < sizeof(legacy_keys_to_new_keys); ++ i) {
            const LegacyKeyToNewKey & mapping = legacy_keys_to_new_keys[i];
            if (mapping.key == ev.key && mapping.player == ev.player) {
                return StateChange {
                    ev.value != 0.0f,
                    mapping.old_name
                };
            }
        }
        LP3_THROW2(lp3::core::Exception, "Bad mapping.");
    }
}

LegacyInputProvider::LegacyInputProvider()
:   changes()
{}

void LegacyInputProvider::handle_events(const SDL_Event & event) {
    if (SDL_KEYDOWN == event.type) {
        StateChange state{ true, key_name_from_sdl(event.key.keysym.sym) };
        auto ev = state_change_to_event(state);
        if (ev) {
            changes.emplace_back(ev.get());
        }
    } else if (SDL_KEYUP == event.type) {
        StateChange state{ false, key_name_from_sdl(event.key.keysym.sym) };
        auto ev = state_change_to_event(state);
        if (ev) {
            changes.emplace_back(ev.get());
        }
    }
}

std::vector<Event> LegacyInputProvider::retrieve_events(std::int64_t) {
    auto old_changes = std::move(changes);
    changes.clear();
    return old_changes;
}


ModernInputProvider::ModernInputProvider(lp3::input::Controls & _controls)
:   controls(_controls),
    memory()
{}


std::vector<Event> ModernInputProvider::retrieve_events(std::int64_t) {
    std::vector<Event> events;

    for (int i = 0; i < 3; ++ i) {
        lp3::input::Control & c = controls.get_control(i);
        auto updates = c.get_updates(memory[i]);
        for (const auto & button : updates) {
            events.push_back(
                Event{
                    lp3::narrow<std::int8_t>(i),
                    static_cast<Key>(button.index),
                    button.value,
                }
            );
        }
        memory[i] = c.remember();
    }

    return std::move(events);
}


InputMultiplexer::InputMultiplexer()
: providers()
{}

void InputMultiplexer::add_input(InputProvider * provider) {
    this->providers.push_back(provider);
}

std::vector<Event> InputMultiplexer::retrieve_events(std::int64_t ms) {
    std::vector<Event> all_changes;
    for (auto & provider : this->providers) {
        auto changes = provider->retrieve_events(ms);
        all_changes.insert(all_changes.end(), changes.begin(), changes.end());
    }
    return all_changes;
}

InputRecorder::InputRecorder(lp3::sdl::RWops && write_file, InputProvider & input)
:   file(std::move(write_file)),
    provider(input),
    time(0)
{
}

std::vector<Event> InputRecorder::retrieve_events(std::int64_t ms) {
    this->time += ms;
    auto events = provider.retrieve_events(ms);
    for (const Event & event : events) {
        StateChange change = event_to_legacy_state(event);
        this->file.write(this->time);
        this->file.write(change.on);
        this->file.write(lp3::narrow<int>(change.key_name.size()));
        this->file.write(change.key_name.c_str(), change.key_name.size());
    }
    return events;
}

InputPlayback::InputPlayback(lp3::sdl::RWops && read_file)
:   file(std::move(read_file)),
    next_time(this->file.read_optional<std::int64_t>()),
    time(0)
{
}

bool InputPlayback::playback_finished() const {
    return !next_time.is_initialized();
}

boost::optional<Event> InputPlayback::read_event() {
    StateChange sc;
    this->file.read(sc.on);

    int string_size;
    this->file.read(string_size);

    sc.key_name.resize(string_size);;
    this->file.read(&sc.key_name.front(), string_size);

    return state_change_to_event(sc);
}

std::vector<Event> InputPlayback::retrieve_events(std::int64_t ms) {
    this->time += ms;

    std::vector<Event> changes;
    while (next_time && next_time.get() <= time) {
        auto ev = this->read_event();
        if (ev) {
            // Ignore events that don't map.
            changes.push_back(ev.get());
        }
        next_time = this->file.read_optional<std::int64_t>();
    }

    return changes;
}


}   }
