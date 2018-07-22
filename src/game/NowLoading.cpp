#include "NowLoading.hpp"

namespace nnd3d { namespace game {


class NowLoadingScreen : public GameProcess {
public:
    NowLoadingScreen(
        GameContext _context,
        std::function<std::unique_ptr<GameProcess>(GameContext context)> _next_proc
    )
    :   context(_context),
        next_proc(_next_proc)
    {
        context.view.load_texture(0, "NowLoading.png", glm::ivec2{ 320, 240 });
        context.view.billboards().resize(1);
        context.view.billboards()[0].set(0, 0, 640, 480, 0, 0, 320, 240);
        context.view.billboards()[0].texture_index = 0;
    }

    ~NowLoadingScreen() override {
    }

    void handle_input(const input::Event &) override {
    }

    std::unique_ptr<GameProcess> update() override {
        return next_proc(context);
    }
private:
    GameContext context;
    std::function<std::unique_ptr<GameProcess> (GameContext context)> next_proc;
};

std::unique_ptr<GameProcess> create_now_loading_screen(
    GameContext context,
    std::function<std::unique_ptr<GameProcess> (GameContext context)> next_proc)
{
    return std::make_unique<NowLoadingScreen>(context, next_proc);
}

}   }
