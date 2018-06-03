
#include "TitleScreen.hpp"
#include <lp3/sims.hpp>
#include "SelectScreen.hpp"

namespace nnd3d { namespace game {

namespace {
    glm::vec4 to_rgb(int r, int g, int b) {
        // Emulates old VB RGB function. Named 'to_rgb' due to probable windows.h
        // issues. >:(
        const glm::vec4 rgb(r/255.0f, g/255.0f, b/255.0f, 1.0f);
        return rgb;
    }
}


class IntroStoryScreen : public GameProcess
{
private:
    GameContext context;
    std::array<bool, 3> keys_pressed;
    lp3::sims::CoroutineState fade_in_and_out_words_state;
    lp3::sims::CoroutineState intro_story_state;
    std::int64_t time;
    int fade_value;
    int fade_length;
    int fade_speed;
    bool fade_in_only;

public:
    IntroStoryScreen(GameContext _context)
    :   context(_context),
        keys_pressed{},
        fade_in_and_out_words_state{},
        intro_story_state{},
        time(0),
        fade_value(0),
        fade_length(0),
        fade_speed(0),
        fade_in_only(false)
    {
        context.view.billboards().resize(3);
        for (auto & b : context.view.billboards()) {
            b.set_visibility(view::Visibility::invisible);
        }
        context.view.billboards().clear();
        context.view.billboards().resize(2);
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

    void fade_in_and_out_words() {
        auto & words = context.view.billboards()[1];

        LP3_COROUTINE_BEGIN(fade_in_and_out_words_state);
        fade_value = 0;

        words.color = view::qb_color(0);

        while (fade_value <= 245) {
            fade_value += fade_speed; // (world.sFactor * s.invTime);
            //0s.3 * sFactor
            words.color = to_rgb(fade_value, fade_value, fade_value);
            LP3_YIELD();
        }
        words.color = view::qb_color(15);

        // 'length' apparently drives how long the word stays up
        // s.miscTime = fade_length;  // world.clock + s.length;

        if (fade_in_only) {
            goto the_end;
        }

        time = 0;
        while (time < fade_length) {
            LP3_YIELD();
        }

        fade_value = 255;

        // Fade out
        while (fade_value >= 1) {
            //LP3_ASSERT(world.sFactor * 20 == 40);
            fade_value -= 38; // = s.miscTime - (world.sFactor * 20); //0.3 * sFactor
            if (fade_value >= 0) {
                words.color = to_rgb(fade_value, fade_value, fade_value);
            }
            LP3_YIELD();
        }

        words.color = view::qb_color(0);

    the_end:
        LP3_COROUTINE_END()
    }

    // Don't judge me for this:
    #define FADE_IN_AND_OUT \
            fade_in_and_out_words_state = {};   \
            while (fade_in_and_out_words_state) {   \
                fade_in_and_out_words();    \
                /*create_billboards(world, view.billboards());  */\
                LP3_YIELD(nullptr);    \
            }

    gsl::owner<GameProcess *> update() override {
        for (int i = 0; i <= 2; ++i) {
            if (this->keys_pressed[i]) {
                return create_select_screen(context, keys_pressed);
            }
        }

        time += ms_per_update;

        if (intro_story_state) {
            auto & bg = context.view.billboards()[0];
            auto & words = context.view.billboards()[1];

            LP3_COROUTINE_BEGIN(intro_story_state);

            // This came from the following expression in the old code:
            //  (world.sFactor * s.invTime);
            // invTime was 40, and later was 10 for a more dramatic fade in
            // I believe 80 is correct, but am not sure.
            fade_speed = 77;
            fade_in_only = false;
            fade_length = 3120;
            bg.z = 0.9f;
            words.z = 0.3f;
            bg.set_visibility(view::Visibility::invisible);

            context.view.LoadTexture(1, "Open1.png", 313, 263);
            context.view.LoadTexture(2, "Open6.png", 320, 258);
            context.view.LoadTexture(3, "Open7.png", 320, 194);
            context.view.LoadTexture(4, "TitleScreen.png", 320, 240);
            context.sound.PlayBgm("");

            words.texture_index = 2;

            words.ul = { 1, 175 };

            context.sound.PlayWave("IntroStory.ogg"); // play it once then stop

            words.ul = { 1, 178 };
            words.size = { 164, 40 }; words.size *= 2;
            words.tex_src_ul = { 2, 1 };
            words.tex_src_dr = { 166, 41 };
            words.set_visibility(view::Visibility::normal);

            FADE_IN_AND_OUT

            words.ul = { 1, 178 };
            words.size = { 301, 152 - 44 }; words.size *= 2;
            words.tex_src_ul = { 2, 44 };
            words.tex_src_dr = { 303, 152 };

            FADE_IN_AND_OUT

            words.ul = { 2, 153 };
            words.size = { 121, 20 }; words.size *= 2;
            words.tex_src_ul = { 2, 153 };
            words.tex_src_dr = { 123, 173 };

            FADE_IN_AND_OUT

            words.ul = { 1, 178 };
            words.size = { 308, 89 }; words.size *= 2;
            words.tex_src_ul = { 3, 174 };
            words.tex_src_dr = { 311, 263 };

            FADE_IN_AND_OUT

            // Show the characters:
            fade_length = 5000;
            context.view.LoadTexture(-1, "Open2.png", 320, 240);

            bg.size = { 640, 480 };
            bg.ul = { 0, 0 };
            bg.tex_src_ul = { 0, 0 };
            bg.tex_src_dr = { 320, 240 };
            bg.set_visibility(view::Visibility::normal);

            words.ul = { 2, 173 }; words.ul *= 2;
            words.size = { 305, 43 }; words.size *= 2;
            words.tex_src_ul = { 2, 5 };
            words.tex_src_dr = { 307, 48 };
            words.texture_index = 3;

            FADE_IN_AND_OUT


            fade_length = 3120;
            words.size = { 309, 20 }; words.size *= 2;
            words.tex_src_ul = { 2, 51 };
            words.tex_src_dr = { 311, 71 };

            FADE_IN_AND_OUT

            words.size = { 293, 42 }; words.size *= 2;
            words.tex_src_ul = { 2, 75 };
            words.tex_src_dr = { 295, 117 };
            fade_length = 4160;

            FADE_IN_AND_OUT

            words.size = { 292, 65 }; words.size *= 2;
            words.tex_src_ul = { 2, 120 };
            words.tex_src_dr = { 294, 185 };
            fade_length = 6240;

            FADE_IN_AND_OUT

            context.view.LoadTexture(-1, "Open3.png", 320, 240);

            words.size = { 303, 65 }; words.size *= 2;
            words.tex_src_ul = { 2, 189 };
            words.tex_src_dr = { 305, 254 };
            fade_length = 6240;

            FADE_IN_AND_OUT

            context.view.LoadTexture(-1, "Open4.png", 320, 240);

            fade_length = 6240;
            words.size = { 313, 65 }; words.size *= 2;
            words.tex_src_ul = { 1, 4 };
            words.tex_src_dr = { 312, 69 };
            words.texture_index = 4;

            FADE_IN_AND_OUT

            context.view.LoadTexture(-1, "Open5.png", 320, 240);

            words.ul = { 58, 188 }; words.ul *= 2;
            words.size = { 259, 43 }; words.size *= 2;
            words.tex_src_ul = { 1, 72 };
            words.tex_src_dr = { 258, 115 };
            fade_length = 6240;

            FADE_IN_AND_OUT

            // Don't bother loading the plainblack texture, just turn the bg off.
            // view.LoadTexture(-1, "PlainBlack.png", 320, 240);
            bg.set_visibility(view::Visibility::invisible);

            words.ul = { 1, 1 };
            words.size = { 320, 240 }; words.size *= 2;
            words.tex_src_ul = { 1, 1 };
            words.tex_src_dr = { 320, 240 };
            words.texture_index = 5;
            fade_length = 6240;

            fade_speed = fade_speed / 4;
            fade_in_only = true;

            FADE_IN_AND_OUT

            LP3_COROUTINE_END()
        }
        return nullptr;
    }
};  // end of GameImpl class


GameProcess * create_intro_story_screen(GameContext context) {
    return new IntroStoryScreen(context);
}

namespace {
    RegisterGameProcess _reg{"intro", "The opening cinematic", 
                             create_intro_story_screen };
}

}   }  // end namespace
