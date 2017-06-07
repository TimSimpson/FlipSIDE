#include <lp3/core.hpp>
#include <lp3/gfx.hpp>
#include <lp3/sims.hpp>
#include <lp3/main.hpp>

#include "Game.hpp"
#include "Sound.hpp"
#include "View.hpp"
#include "World.hpp"

namespace core = lp3::core;
namespace gfx = lp3::gfx;
namespace sims = lp3::sims;
namespace sdl = lp3::sdl;


int _main(core::PlatformLoop & loop) {
	sdl::SDL2 sdl2(SDL_INIT_VIDEO);
	core::LogSystem log;
	core::MediaManager media;

	gfx::Window window("FlipSIDE", glm::vec2{ 640, 480 });

	nnd3d::View view{ media };
	nnd3d::Sound sound;
	(void)sound;

	sims::FrameTimer frame_timer;

	nnd3d::World world;
	nnd3d::Game game(view, sound, world);

	const std::int64_t ms_per_update = 1000 / 60;  //16 ms for 60 fps
	sims::GameClock clock(ms_per_update);

	return loop.run([&]() {
		bool quit = false;
		SDL_Event e;
		if (SDL_PollEvent(&e)) {
			switch (e.type) {
			case SDL_QUIT:
				quit = true;
				break;
			case SDL_WINDOWEVENT:
				window.handle_events(e.window);
				break;
			}
		}
		// controls update
		clock.run_updates([&world, &game](std::int64_t ms) {
			// The old code figured out the percentage of a second each frame
			// took, and created a "speed factor" which it multiplied everything
			// in existence by. That's actually a bad approach for several
			// reasons, but the take away is here we introducing a constant
			// speed mod which will always be 0.016
			world.sFactor = lp3::narrow<double>(ms) / 1000.0;
			if (world.LemonTime) {
				world.sFactor *= 2;
			}
			// TODO(2017): I think the target FPS changed from 120 to 60... 
			// double check this maybe.
			world.clock = world.clock + ((1 / 120) * world.sFactor);
			game.PlayGame();
		});

		frame_timer.next_frame();		
		view.DrawStuff(frame_timer.get_fps());
		window.render(std::ref(view));
		return !quit;
	});
}

LP3_MAIN(_main)
