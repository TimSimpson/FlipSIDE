#include "TitleScreen.hpp"
#include <boost/algorithm/string/predicate.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <lp3/sims.hpp>
#include "BaseScreen.hpp"
#include "CharacterProc.hpp"
#include "SelectScreen.hpp"

#define WAIT(ms) while(time < ms) { LP3_YIELD(); }


#ifdef _MSC_VER
    // Avoid the zillions implicit conversion warnings
    #pragma warning(disable: 4244)
#endif

// TODO: Remove gotos!
#define EMPTY_LABEL_HACK  { constexpr int dumb = 1; LP3_LOG_VAR(dumb) }

namespace nnd3d { namespace game {

namespace {
    const glm::vec4 normColor{1.0f, 1.0f, 1.0f, 1.0f};

    glm::vec4 to_rgb(int r, int g, int b) {
        // Emulates old VB RGB function. Named 'to_rgb' due to probable windows.h
        // issues. >:(
        const glm::vec4 rgb(r/255.0f, g/255.0f, b/255.0f, 1.0f);
        // LP3_ASSERT(rgb.r == r && rgb.g == g && rgb.b == b);
        return rgb;
    }
}


class TitleScreenImpl : public GameProcess
{
private:
	Vb & vb;
	view::View & view;
	Sound & sound;
	World world;
	std::array<bool, 3> keys_pressed;
	lp3::sims::CoroutineState coro_state;
	lp3::sims::CoroutineState titles_coro_state;
	lp3::sims::CoroutineState fade_in_and_out_words_state;
	lp3::sims::CoroutineState intro_story_state;
	std::int64_t time;
	std::int64_t fade_value;
	std::int64_t fade_length;
	std::int64_t fade_speed;
	bool fade_in_only;

public:
	TitleScreenImpl(view::View & view_arg, Sound & sound_arg, Vb & vb_arg)
    :   vb(vb_arg),
        view(view_arg),
        sound(sound_arg),
		world{},
		keys_pressed{},
		coro_state(),
		titles_coro_state{},
		fade_in_and_out_words_state{},
		intro_story_state{},
		time(0),
		fade_value(0),
		fade_length(0),
		fade_speed(0),
		fade_in_only(false)
    {
		view.billboards().resize(3);
		for (auto & b : view.billboards()) {
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

	void opening_titles() {
		if (!titles_coro_state) {
			return;
		}

		view::Billboard & words = view.billboards()[0];
		words.z = 0.3f;
		view::Billboard & title_bg_1 = view.billboards()[1];
		title_bg_1.z = 0.9f;
		view::Billboard & subtitle = view.billboards()[2];
		subtitle.z = 0.4f;
				
		LP3_COROUTINE_BEGIN(titles_coro_state);

		view.LoadTexture(0, "title2ALT.png", 285, 427);
		view.LoadTexture(1, "title1.png", 440, 427);
		sound.PlayWave("OpeningWAV.wav");
		
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
				subtitle.size.x -= (5 * speed_factor);
				subtitle.ul.x += (2.5 * speed_factor);
			} else {
				subtitle.set_visibility(view::Visibility::normal);
			}
			words.size.y -= (5 * speed_factor);
			words.ul.y += (2.5 * speed_factor);
			LP3_YIELD();
		}
		words.set_visibility(view::Visibility::normal);

		words.set_visibility(view::Visibility::normal);
		subtitle.set_visibility(view::Visibility::normal);

		WAIT(23072)

		world.screen = "intro story";
			
		LP3_COROUTINE_END()
	}

	void fade_in_and_out_words() {
		auto & bg = view.billboards()[0];
		auto & words = view.billboards()[1];
		
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

	void intro_story() {
		auto & bg = view.billboards()[0];
		auto & words = view.billboards()[1];

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
			
		view.LoadTexture(1, "Open1.png", 313, 263);
		view.LoadTexture(2, "Open6.png", 320, 258);
		view.LoadTexture(3, "Open7.png", 320, 194);
		view.LoadTexture(4, "TitleScreen.png", 320, 240);
		sound.PlayBgm("");
		
		words.texture_index = 2;

		words.ul = { 1, 175 };

		sound.PlayWave("IntroStory.ogg"); // play it once then stop

		words.ul = { 1, 178 };
		words.size = { 164, 40 }; words.size *= 2;
		words.tex_src_ul = { 2, 1 };
		words.tex_src_dr = { 166, 41 };
		words.set_visibility(view::Visibility::normal);
				

		// Don't judge me for this:
#define FADE_IN_AND_OUT \
			fade_in_and_out_words_state = {};	\
			while (fade_in_and_out_words_state) {	\
				fade_in_and_out_words();	\
				/*create_billboards(world, view.billboards());	*/\
				LP3_YIELD();	\
			}

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
		view.LoadTexture(-1, "Open2.png", 320, 240);

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

		view.LoadTexture(-1, "Open3.png", 320, 240);

		words.size = { 303, 65 }; words.size *= 2;
		words.tex_src_ul = { 2, 189 };
		words.tex_src_dr = { 305, 254 };
		fade_length = 6240;

		FADE_IN_AND_OUT

		view.LoadTexture(-1, "Open4.png", 320, 240);

		fade_length = 6240;
		words.size = { 313, 65 }; words.size *= 2;
		words.tex_src_ul = { 1, 4 };
		words.tex_src_dr = { 312, 69 };
		words.texture_index = 4;

		FADE_IN_AND_OUT

		view.LoadTexture(-1, "Open5.png", 320, 240);

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

	gsl::owner<GameProcess *> update() override {
		for (int i = 0; i <= 2; ++i) {
			if (this->keys_pressed[i]) {
				return create_select_screen(view, sound, vb, keys_pressed);
			}
		}

		time += ms_per_update;
	
		if (coro_state) {
			LP3_COROUTINE_BEGIN(coro_state);
				titles_coro_state = {};
				while (titles_coro_state) {
					opening_titles();
					LP3_YIELD(nullptr);
				}

				view.billboards().clear();
				view.billboards().resize(2);

				intro_story_state = {};
				while (intro_story_state) {
					intro_story();
					LP3_YIELD(nullptr);
				}			
			LP3_COROUTINE_END()		
        }
		return nullptr;
    }
};  // end of GameImpl class


GameProcess * create_title_screen(view::View & view, Sound & sound, Vb & vb) {
	return new TitleScreenImpl(view, sound, vb);
}

}   }  // end namespace
