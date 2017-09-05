#include "GameOverScreen.hpp"
#include <boost/algorithm/string/predicate.hpp>
#include <boost/lexical_cast.hpp>
#include <lp3/sims.hpp>

#include "BaseScreen.hpp"
#include "TitleScreen.hpp"

#ifdef _MSC_VER
    // Avoid the zillions implicit conversion warnings
    #pragma warning(disable: 4244)
#endif

// TODO: Remove gotos!
#define EMPTY_LABEL_HACK  { constexpr int dumb = 1; LP3_LOG_VAR(dumb) }

namespace nnd3d { namespace game {

namespace {
    const glm::vec4 normColor{1.0f, 1.0f, 1.0f, 1.0f};
}

namespace sims = lp3::sims;

class GameOverScreen : public GameProcess
{
private:
    Vb & vb;
    view::View & view;
    Sound & sound;
    Random random;
	
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
    GameOverScreen(view::View & view_arg,
               Sound & sound_arg, Vb & vb_arg)
    :   vb(vb_arg),
        view(view_arg),
        sound(sound_arg),
        random(),
		allocate_billboards(view.billboards()),
		game_over_cloud_bg(view.billboards()[0]),
		game_over_title(view.billboards()[1]),
		wait_time(4000)
    {	
		sound.silence_sfx();
        sound.PlayBgm("");
        view.LoadTexture(0, "GameOver.png", 320, 287);
		
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

        sound.PlayWave("GameOver.wav");
    }

    void handle_input(const input::Event &) override {
    }

    gsl::owner<GameProcess *> update() override {
		LP3_COROUTINE_BEGIN(coro_state);
		while (wait_time > 0) {
			wait_time -= 16;
			LP3_YIELD(nullptr);
		}
		while (game_over_title.ul.y >= -300) {
			game_over_title.size.y += lp3::narrow<float>(2.0f * speed_factor);
			game_over_title.ul.y -= lp3::narrow<float>(speed_factor);
			if (game_over_title.ul.y < 0) {
				game_over_title.set_visibility(view::Visibility::flicker);
			}
			LP3_YIELD(nullptr);
		}
		while (game_over_title.size.x >= 0) {
			game_over_title.size.x -= lp3::narrow<float>(10.0f * speed_factor);
			game_over_title.ul.x += lp3::narrow<float>(5.0f * speed_factor);
			LP3_YIELD(nullptr);
		}
		LP3_COROUTINE_END();
		return create_title_screen(view, sound, vb);
    }



};  // end of GameImpl class


gsl::owner<GameProcess *> create_gameover_screen(
    view::View & view, Sound & sound, Vb & vb)
{
    return new GameOverScreen(view, sound, vb);
}

}   }  // end namespace
