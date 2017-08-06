#include <chrono>

#include <lp3/core.hpp>
#include <lp3/gfx.hpp>
#include <lp3/input.hpp>
#include <lp3/sims.hpp>
#include <lp3/main.hpp>

#include "Game.hpp"
#include "Input.hpp"
#include "Sound.hpp"
#include "Vb.hpp"
#include "View.hpp"
#include "World.hpp"

namespace core = lp3::core;
namespace gfx = lp3::gfx;
namespace input = lp3::input;
namespace mix = lp3::mix;
namespace sims = lp3::sims;
namespace sdl = lp3::sdl;


struct CommandLineArgs {
	bool skip_playback_to_end;
	boost::optional<std::string> record_input;
	boost::optional<std::string> playback_input;
};

boost::optional<CommandLineArgs> parse_args(
	const std::vector<std::string> & string_args)
{
	CommandLineArgs args;
	args.skip_playback_to_end = false;

	for (std::size_t i = 1; i < string_args.size(); ++i) {
		if (string_args[i] == "--record-input") {
			if (i + 1 >= string_args.size()) {
				std::cerr << "Expected a file name for record-input.\n";
				return boost::none;
			}
			args.record_input = string_args[i + 1];
			++i;
		} else if (string_args[i] == "--playback-input") {
			if (i + 1 >= string_args.size()) {
				std::cerr << "Expected a file name for playback-input.\n";
				return boost::none;
			}
			args.playback_input = string_args[i + 1];
			++i;
		} else if (string_args[i] == "--skip-playback-to-end") {
			if (!args.playback_input) {
				std::cerr << "--skip-playback-to-end requires option --playback-input\n";
				return boost::none;
			}
			args.skip_playback_to_end = true;
		} else {
			std::cerr << "Unknown argument: " << string_args[i] << "\n"
				"Options: \n"
				"    --record-input <file-to-write>\n"
				"    --playback-input <file-to-read>\n"
				"    --skip-playback-to-end\n";
			return boost::none;
		}
	}
	return args;
}

int _main(core::PlatformLoop & loop) {
	const auto start_time = std::chrono::high_resolution_clock::now();

	sdl::SDL2 sdl2(SDL_INIT_VIDEO);
	core::LogSystem log;

	auto op_args = parse_args(loop.command_line_args());
	if (!op_args) {
		return 1;
	}
	auto args = op_args.get();

	core::MediaManager base_media;
	core::MediaManager media = base_media.sub_directory(
		#ifndef LP3_COMPILE_TARGET_EMSCRIPTEN
			"FlipSIDE"
		#else
			"FlipSIDE" //-lite"
		#endif
	);
	input::Controls controls;

	// Set up input stuff
	nnd3d::input::KeyboardInputProvider kb_input;
	nnd3d::input::InputMultiplexer input_multiplexer;
	input_multiplexer.add_input(&kb_input);
	nnd3d::input::InputProvider * input = &input_multiplexer;

	// Playback input if requested
	std::unique_ptr<nnd3d::input::InputPlayback> playback;
	if (args.playback_input) {
		LP3_LOG_INFO("Loading input playback file %s...", *args.playback_input);
		auto file = base_media.load(*args.playback_input);
		playback.reset(new nnd3d::input::InputPlayback(std::move(file)));
		input_multiplexer.add_input(playback.get());
	}

	// Set up input recorder if requested
	std::unique_ptr<nnd3d::input::InputRecorder> recorder;
	if (args.record_input) {
		LP3_LOG_INFO("Recording input to file %s...", *args.record_input);
		auto file = base_media.save(*args.record_input);
		recorder.reset(new nnd3d::input::InputRecorder(std::move(file), *input));
		input = recorder.get();
	}

	gfx::Window window("FlipSIDE", glm::vec2{ 640, 480 });

	glEnable(GL_DEPTH_TEST);  // DOES SOMETHING, makes Z count between render calls

	#if LP3_COMPILE_TARGET_EMSCRIPTEN
		mix::Init mix_init();
	#else
		mix::Init mix_init(MIX_INIT_MP3);
	#endif

	nnd3d::Vb vb{ media };
	nnd3d::World world;
	nnd3d::View view{ media, world };

	nnd3d::MutableSound sound{ media };
	(void)sound;

	nnd3d::Game game(view, sound, vb, world);

	sims::FrameTimer frame_timer;

    // 2017: This clock runs all the old game logic.
    //       In the old code, this ran in an endless loop which called
    //       `doEvents` and used a suspect method of calculating the percentage
    //       of a second each frame took. Here we can just use a GameClock.
	const std::int64_t ms_per_update = 1000 / 60;  //16 ms for 60 fps
	sims::GameClock clock(ms_per_update);

    // 2017: This next clock stands in for a Visual Basic Timer that the
    //       old code would fire once every 200 ms.
    sims::GameClock old_timer(200);

	auto run_game = [&world, &game, &input, &sound](std::int64_t ms) {
		// Garbage collect sound to avoid out of channel problems.
		sound.garbage_collect();

		// Handle input
		{
			auto changes = input->retrieve_events(ms);
			for (const auto & change : changes) {
				if (change.on) {
					game.OnKey(change.key_name);
				}
				else {
					game.OffKey(change.key_name);
				}
			}
		}

		// The old code figured out the percentage of a second each frame
		// took, and created a "speed factor" which it multiplied everything
		// in existence by. That's actually a bad approach for several
		// reasons, but the take away is here we introducing a constant
		// speed mod which will always be 0.016
		world.sFactor = lp3::narrow<double>(ms) / 1000.0;
		world.clock = world.clock + world.sFactor;
		if (world.LemonTime) {
			world.sFactor *= 2;
		}
		// So the game created a speed factor that tried to make it
		// target 120fps (kind of cool my old Pentium 2 machine could do
		// that). So we multiple the number we just had by 120.
		world.sFactor *= 120;

		game.PlayGame();
	};

	if (args.skip_playback_to_end) {
		// Speed through the game loop until the end of playback.
		sound.mute();
		view.disable();
		while (!playback->playback_finished()) {
			run_game(ms_per_update);
		}
		const auto playback_end_time = std::chrono::high_resolution_clock::now();
		const auto playback_time =
			std::chrono::duration_cast<std::chrono::milliseconds>(
				playback_end_time - start_time).count();
#ifndef LP3_COMPILE_WITH_DEBUGGING
		std::cout << "Playback completed in " << playback_time << "ms.\n";
#endif
		LP3_LOG_INFO("Playback completed in %d ms", playback_time);

		sound.unmute();
		view.enable();
	}

	const auto result = loop.run([&]() {
		bool quit = world.STOPGAME;
		SDL_Event e;
		while (SDL_PollEvent(&e)) {
			switch (e.type) {
			case SDL_QUIT:
				quit = true;
				break;
			case SDL_WINDOWEVENT:
				window.handle_events(e.window);
				break;
			default:
				break;
			}
			kb_input.handle_events(e);
		}
		controls.update();

		clock.run_updates(run_game);

		old_timer.run_updates([&game](std::int64_t) {
			game.TimedEvents();
		});

		frame_timer.next_frame();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		view.DrawStuff(frame_timer.get_fps());
		window.render(std::ref(view));
		return !quit;
	});

	const auto end_time = std::chrono::high_resolution_clock::now();
	const auto process_time =
		std::chrono::duration_cast<std::chrono::milliseconds>(
			end_time - start_time).count();
	LP3_LOG_INFO("Process ran for %d ms", process_time);
	return result;
}

LP3_MAIN(_main)
