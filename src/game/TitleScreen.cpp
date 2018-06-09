#include "TitleScreen.hpp"
#include <lp3/sims.hpp>
#include "IntroStory.hpp"
#include "SelectScreen.hpp"


namespace nnd3d { namespace game {


class TitleScreenImpl : public GameProcess
{
private:
    GameContext context;
    std::array<bool, 3> keys_pressed;
    lp3::sims::CoroutineState coro_state;
    std::int64_t time;

public:
    TitleScreenImpl(GameContext _context)
    :   context(_context),
        keys_pressed{},
        coro_state(),
        time(0)
    {
        context.view.billboards().resize(3);
        for (auto & b : context.view.billboards()) {
            b.set_visibility(view::Visibility::invisible);
        }
    }

    void handle_input(const input::Event & event) override {
        switch(event.key) {
            case input::Key::up:
            case input::Key::down:
            case input::Key::left:
            case input::Key::right:
            case input::Key::attack:
            case input::Key::jump:
                if (event.value) {
                    keys_pressed[event.player] = true;
                }
                break;
            default:
                break;
        }
    }

    #define WAIT(ms) while(time < ms) { LP3_YIELD(nullptr); }

    gsl::owner<GameProcess *> update() override {
        for (int i = 0; i <= 2; ++i) {
            if (this->keys_pressed[i]) {
                return create_select_screen(context, keys_pressed);
            }
        }

        time += ms_per_update;

        if (coro_state) {
            view::Billboard & words = context.view.billboards()[0];
            words.z = 0.3f;
            view::Billboard & title_bg_1 = context.view.billboards()[1];
            title_bg_1.z = 0.9f;
            view::Billboard & subtitle = context.view.billboards()[2];
            subtitle.z = 0.4f;

            LP3_COROUTINE_BEGIN(coro_state);

            context.view.LoadTexture(0, "title2ALT.png", 285, 427);
            context.view.LoadTexture(1, "title1.png", 440, 427);
            context.sound.PlayWave("OpeningWAV.wav");

            words.set_visibility(view::Visibility::invisible);
            WAIT(2 * 1000)
                words.set_visibility(view::Visibility::normal);
            words.tex_src_ul = { 1, 3 };
            words.tex_src_dr = { 196, 107 };
            words.ul = { 0, 180 };
            words.size = { 193, 106 };
            words.texture_index = 1;

            WAIT(5 * 1000)
                words.set_visibility(view::Visibility::invisible);

            WAIT(6 * 1000)
                words.set_visibility(view::Visibility::normal);
            words.tex_src_ul = { 2, 111 };
            words.tex_src_dr = { 279, 230 };
            words.ul = { 0, 174 };
            words.size = { 277, 119 };

            WAIT(9 * 1000)
                words.set_visibility(view::Visibility::invisible);

            WAIT(10 * 1000)
                words.set_visibility(view::Visibility::normal);
            words.tex_src_ul = { 1, 233 };
            words.tex_src_dr = { 224, 363 };
            words.ul = { 0, 168 };
            words.size = { 232, 130 };

            WAIT(13 * 1000)
                words.set_visibility(view::Visibility::invisible);

            WAIT(14 * 1000)
                words.set_visibility(view::Visibility::normal);
            words.tex_src_ul = { 1, 366 };
            words.tex_src_dr = { 198, 424 };
            words.ul = { 0, 228 };
            words.size = { 197, 58 };

            WAIT(17 * 1000)
                words.set_visibility(view::Visibility::invisible);

            WAIT(20 * 1000)
                // Show background, have the title and subtitle be stretched and
                // pull in to the right size.
                title_bg_1.set_visibility(view::Visibility::normal);
            title_bg_1.tex_src_ul = { 5, 137 };
            title_bg_1.tex_src_dr = { 324, 318 };
            title_bg_1.ul = { 1, 1 };
            title_bg_1.size = { 640, 480 };
            title_bg_1.texture_index = 2;
            title_bg_1.z = 0.99f;

            subtitle.set_visibility(view::Visibility::flicker);
            subtitle.tex_src_ul = { 7, 91 };
            subtitle.tex_src_dr = { 437, 128 };
            subtitle.ul = { -320, 140 + 213 };
            subtitle.size = { 1280, 110 };
            subtitle.texture_index = 2;
            subtitle.z = 0.75f;

            words.set_visibility(view::Visibility::flicker);
            words.tex_src_ul = { 9, 6 };
            words.tex_src_dr = { 348, 81 };
            words.ul = { 1, -240 };
            words.size = { 640, 960 };
            words.texture_index = 2;
            words.z = 0.5f;


            while (words.size.y >= 213) {
                if (subtitle.size.x > 640) {
                    subtitle.size.x -= (5 * fs_speed_factor);
                    subtitle.ul.x += (2.5 * fs_speed_factor);
                }
                else {
                    subtitle.set_visibility(view::Visibility::normal);
                }
                words.size.y -= (5 * fs_speed_factor);
                words.ul.y += (2.5 * fs_speed_factor);
                LP3_YIELD(nullptr);
            }
            words.set_visibility(view::Visibility::normal);

            words.set_visibility(view::Visibility::normal);
            subtitle.set_visibility(view::Visibility::normal);

            WAIT(23072)

            return create_intro_story_screen(context);
            LP3_COROUTINE_END()
        }
        return nullptr;
    }
};  // end of GameImpl class


GameProcess * create_title_screen(GameContext context) {
    return new TitleScreenImpl(context);
}

namespace {
    RegisterGameProcess _reg{ "title", "Opening titles", create_title_screen };
}

}   }  // end namespace
