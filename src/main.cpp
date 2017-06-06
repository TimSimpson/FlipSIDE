#include <lp3/core.hpp>
#include <lp3/gfx.hpp>
#include <lp3/sims.hpp>
#include <lp3/main.hpp>

namespace core = lp3::core;
namespace gfx = lp3::gfx;
namespace sims = lp3::sims;
namespace sdl = lp3::sdl;


int _main(core::PlatformLoop & loop) {
    sdl::SDL2 sdl2(SDL_INIT_VIDEO);
    core::LogSystem log;
    core::MediaManager media;

    gfx::Window window("FlipSIDE", glm::vec2{ 640, 480 });

    sims::FrameTimer frame_timer;

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
        frame_timer.next_frame();
        LP3_LOG_DEBUG("FPS: %d\tAvg FPS: %d",
                      frame_timer.get_fps(),
                      frame_timer.get_average_fps());
        return !quit;
    });
}

LP3_MAIN(_main)
