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
}

KeyboardInputProvider::KeyboardInputProvider()
:	changes()
{}

void KeyboardInputProvider::handle_events(const SDL_Event & event) {
	if (SDL_KEYDOWN == event.type) {
		changes.emplace_back(
			StateChange{ true, key_name_from_sdl(event.key.keysym.sym) });
	} else if (SDL_KEYUP == event.type) {
		changes.emplace_back(
			StateChange{ false, key_name_from_sdl(event.key.keysym.sym) });
	}
}

std::vector<StateChange> KeyboardInputProvider::retrieve_events(std::int64_t ms) {
	LP3_LOG_VAR(ms);
	auto old_changes = std::move(changes);
	changes.clear();
	return old_changes;
}

InputMultiplexer::InputMultiplexer()
: providers()
{}

void InputMultiplexer::add_input(InputProvider * provider) {
	this->providers.push_back(provider);
}

std::vector<StateChange> InputMultiplexer::retrieve_events(std::int64_t ms) {
	std::vector<StateChange> all_changes;
	for (auto & provider : this->providers) {
		auto changes = provider->retrieve_events(ms);
		all_changes.insert(all_changes.end(), changes.begin(), changes.end());
	}
	return all_changes;
}

InputRecorder::InputRecorder(lp3::sdl::RWops && write_file, InputProvider & input)
:	file(std::move(write_file)),
	provider(input),
	time(0)
{
}

std::vector<StateChange> InputRecorder::retrieve_events(std::int64_t ms) {
	this->time += ms;
	auto events = provider.retrieve_events(ms);
	for (const StateChange & change : events) {
		this->file.write(this->time);
		this->file.write(change.on);
		this->file.write(lp3::narrow<int>(change.key_name.size()));
		this->file.write(change.key_name.c_str(), change.key_name.size());
	}
	return events;
}

InputPlayback::InputPlayback(lp3::sdl::RWops && read_file)
:	file(std::move(read_file)),
	next_time(this->file.read_optional<std::int64_t>()),
	time(0)
{
}

bool InputPlayback::playback_finished() const {
	return !next_time.is_initialized();
}

StateChange InputPlayback::read_event() {
	StateChange sc;
	this->file.read(sc.on);

	int string_size;
	this->file.read(string_size);

	sc.key_name.resize(string_size);;
	this->file.read(&sc.key_name.front(), string_size);

	return sc;
}

std::vector<StateChange> InputPlayback::retrieve_events(std::int64_t ms) {
	this->time += ms;

	std::vector<StateChange> changes;
	while (next_time && next_time.get() <= time) {
		changes.push_back(this->read_event());
		next_time = this->file.read_optional<std::int64_t>();
	}

	return changes;
}


}   }
