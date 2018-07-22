#include "GameOverScreen.hpp"
#include <boost/algorithm/string/predicate.hpp>
#include <boost/lexical_cast.hpp>
#include <lp3/sims.hpp>

#include "TitleScreen.hpp"

namespace nnd3d { namespace game {

namespace sims = lp3::sims;

class GameOverScreen : public GameProcess
{
private:
    GameContext context;

    struct AllocateBillboards {
        AllocateBillboards(std::vector<view::Billboard> & billboards) {
            billboards.resize(2);
        }
    } allocate_billboards;

    view::Billboard & game_over_cloud_bg;
    view::Billboard & game_over_title;
    std::int64_t wait_time;

    sims::CoroutineState coro_state;
public:
    GameOverScreen(GameContext _context)
    :   context(_context),
        allocate_billboards(context.view.billboards()),
        game_over_cloud_bg(context.view.billboards()[0]),
        game_over_title(context.view.billboards()[1]),
        wait_time(4000)
    {
        context.sound.silence_sfx();
        context.sound.PlayBgm("");
        context.view.LoadTexture(0, "GameOver.png", 320, 287);

        game_over_cloud_bg.ul = { 0, 0 };
        game_over_cloud_bg.size = { 640, 480 };
        game_over_cloud_bg.tex_src_ul = { 1, 1 };
        game_over_cloud_bg.tex_src_dr = { 320, 240 };
        game_over_cloud_bg.texture_index = 1;
        game_over_cloud_bg.z = 0.9f;

        game_over_title.ul = { 0, 180 };
        game_over_title.size = { 640, 94 };
        game_over_title.tex_src_ul = { 1, 243 };
        game_over_title.tex_src_dr = { 320, 287 };
        game_over_title.texture_index = 1;
        game_over_title.z = 0.5f;

        context.sound.PlayWave("GameOver.wav");
    }

    void handle_input(const input::Event &) override {
    }

    std::unique_ptr<GameProcess> update() override {
        LP3_COROUTINE_BEGIN(coro_state);
        while (wait_time > 0) {
            wait_time -= 16;
            LP3_YIELD(nullptr);
        }
        while (game_over_title.ul.y >= -300) {
            game_over_title.size.y += lp3::narrow<float>(2.0f * fs_speed_factor);
            game_over_title.ul.y -= lp3::narrow<float>(fs_speed_factor);
            if (game_over_title.ul.y < 0) {
                game_over_title.set_visibility(view::Visibility::flicker);
            }
            LP3_YIELD(nullptr);
        }
        while (game_over_title.size.x >= 0) {
            game_over_title.size.x -= lp3::narrow<float>(10.0f * fs_speed_factor);
            game_over_title.ul.x += lp3::narrow<float>(5.0f * fs_speed_factor);
            LP3_YIELD(nullptr);
        }
        LP3_COROUTINE_END();
        return create_title_screen(context);
    }

};  // end of GameImpl class


std::unique_ptr<GameProcess> create_gameover_screen(GameContext context)
{
    return std::make_unique<GameOverScreen>(context);
}

namespace {
    RegisterGameProcess reg{
        "game-over",  "Game Over screen", create_gameover_screen };
}

}   }  // end namespace
