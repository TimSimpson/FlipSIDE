#include <lp3/core.hpp>
#include <lp3/gfx.hpp>
#include <lp3/input.hpp>
#include <lp3/sims.hpp>
#include <lp3/main.hpp>

#include "Game.hpp"
#include "Sound.hpp"
#include "Vb.hpp"
#include "View.hpp"
#include "World.hpp"

namespace core = lp3::core;
namespace gfx = lp3::gfx;
namespace input = lp3::input;
namespace sims = lp3::sims;
namespace sdl = lp3::sdl;

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

int _main(core::PlatformLoop & loop) {
	sdl::SDL2 sdl2(SDL_INIT_VIDEO);
	core::LogSystem log;
	core::MediaManager media;
	input::Controls controls;
	gfx::Window window("FlipSIDE", glm::vec2{ 640, 480 });
	
	
	nnd3d::Vb vb;	
	nnd3d::World world;
	nnd3d::View view{ media, world };
	nnd3d::Sound sound{ media };
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

	return loop.run([&]() {
		bool quit = world.STOPGAME;
		SDL_Event e;
		if (SDL_PollEvent(&e)) {
			switch (e.type) {
			case SDL_QUIT:
				quit = true;
				break;
			case SDL_WINDOWEVENT:
				window.handle_events(e.window);
				break;
            case SDL_KEYDOWN:
				game.OnKey(key_name_from_sdl(e.key.keysym.sym));
                break;
			case SDL_KEYUP:
				game.OffKey(key_name_from_sdl(e.key.keysym.sym));
				break;
			}
		}
		controls.update();

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

		old_timer.run_updates([&game](std::int64_t ms) {
			game.TimedEvents();
		});

		frame_timer.next_frame();
		view.DrawStuff(frame_timer.get_fps());
		window.render(std::ref(view));
		return !quit;
	});
}

LP3_MAIN(_main)
