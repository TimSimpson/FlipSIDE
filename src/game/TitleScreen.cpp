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

    glm::ivec4 to_rgb(int r, int g, int b) {
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
	std::int64_t time;

public:
	TitleScreenImpl(view::View & view_arg, Sound & sound_arg, Vb & vb_arg)
    :   vb(vb_arg),
        view(view_arg),
        sound(sound_arg),
		world{},
		keys_pressed{},
		coro_state(),
		time(0)
    {
		destroyEverything(world, view, sound);
        world.screen = "title2";
		destroyEverything(world, view, sound);
		sound.PlayWave("OpeningWAV.wav");

		view.LoadTexture(0, "title2ALT.png", 285, 427);
		view.LoadTexture(1, "title1.png", 440, 427);
		
		this->findOrder();

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
		view::Billboard & title_bg_1 = view.billboards()[1];
		view::Billboard & subtitle = view.billboards()[2];
			
		LP3_COROUTINE_BEGIN(titles_coro_state);
			
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
			
		while (subtitle.size.x >= 640) {
			if (words.size.y >= 213) {
				words.size.y -= (5 * speed_factor);
				words.ul.y += (2.5 * speed_factor);
			} else {
				LP3_ASSERT(false);
			}

			subtitle.size.x -= (5 * speed_factor);
			subtitle.ul.x += (2.5 * speed_factor);
			LP3_YIELD();
		}

		while (words.size.y >= 213) {
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

	gsl::owner<GameProcess *> update() override {
		for (int i = 0; i <= 2; ++i) {
			if (this->keys_pressed[i]) {
				return create_select_screen(view, sound, vb, keys_pressed);
			}
		}

		time += ms_per_update;

		set_time_stuff(world);
		flicker(world);
		world.lasttime = world.clock + 3.33333333333333E-02;

		if (coro_state) {
			LP3_COROUTINE_BEGIN(coro_state);
			while (titles_coro_state) {
				opening_titles();
				LP3_YIELD(nullptr);
			}			

			world.screen = "intro story 2";
			destroyEverything(world, view, sound);
			view.LoadTexture(1, "Open1.png", 313, 263);
			view.LoadTexture(2, "Open6.png", 320, 258);
			view.LoadTexture(3, "Open7.png", 320, 194);
			view.LoadTexture(4, "TitleScreen.png", 320, 240);
			sound.PlayBgm("");
			world.Sprite[0].name = "intro story";
			{
				auto & s = world.Sprite[1];
				s.name = "words1";
				s.x = 1;
				s.y = 175;
				s.miscTime = 0;
				s.mode = "words1";
				s.visible = false;
				s.color = view::qb_color(0);
			}

			world.Sprite[0].color = view::qb_color(0);
			world.Sprite[0].visible = false;

			LP3_YIELD(nullptr)

			LP3_COROUTINE_END()
		}

        
        int k = 0;

		
        


        for (int j = 0; j < world.spritesInUse; ++j) {
            auto & s = world.Sprite[j];

            if (s.name == "intro story") {
               
                if (s.mode == "") {

                    world.Sprite[1].invTime = 40;
                    sound.PlayWave("IntroStory.ogg"); // play it once then stop
                    s.mode = "waka do";  // so it won't load non stop
                }
                if (world.Sprite[1].mode == "words1") {
                    //Sprite(1).length = 6
                    if (s.mode == "waka do") {
                        //playWave "conSix.wav"
                        auto & s2 = world.Sprite[1];
                        s2.length = 3;
                        s2.texture = 1;
                        s2.visible = true;
                        //s2.srcx = 2: s2.srcy = 44;
                        //s2.srcx2 = 303: s2.srcy2 = 152;
                        s2.srcx = 2;
                        s2.srcy = 1;
                        s2.srcx2 = 166;
                        s2.srcy2 = 41;
                        s2.wide = 164 * 2;
                        s2.high = 40 * 2;
                        s2.x = 1;
                        s2.y = 178;
                    }
                    if (s.mode == "word 2") {
                        auto & s2 = world.Sprite[1];
                        s2.length = 6;
                        s2.texture = 1;
                        s2.visible = true;
                        s2.srcx = 2;
                        s2.srcy = 44;
                        s2.srcx2 = 303;
                        s2.srcy2 = 152;
                        s2.wide = 301 * 2;
                        s2.high = (152 - 44) * 2;
                        s2.x = 1;
                        s2.y = 178;
                    }
                    if (s.mode == "word 3") {
                        auto & s2 = world.Sprite[1];
                        s2.length = 2;
                        s2.texture = 1;
                        s2.visible = true;
                        s2.srcx = 2;
                        s2.srcy = 153;
                        s2.srcx2 = 123;
                        s2.srcy2 = 173;
                        s2.wide = 121 * 2;
                        s2.high = (20) * 2;
                        s2.x = 1;
                        s2.y = 178;
                    }
                    if (s.mode == "word 4") {
                        auto & s2 = world.Sprite[1];
                        s2.length = 6;
                        s2.texture = 1;
                        s2.visible = true;
                        s2.srcx = 3;
                        s2.srcy = 174;
                        s2.srcx2 = 311;
                        s2.srcy2 = 263;
                        s2.wide = 308 * 2;
                        s2.high = (89) * 2;
                        s2.x = 1;
                        s2.y = 178;
                    }
                    if (s.mode == "word 5") {
                        s.length = 5;
                        view.LoadTexture(-1, "Open2.png", 320, 240);
                        world.camera.CameraWidth = 320;
                        world.camera.CameraHeight = 240;
                        {
                            auto & s2 = world.Sprite[1];
                            s2.texture = 2; //1
                            s2.visible = true;
                            s2.srcx = 2;
                            s2.srcy = 5;
                            s2.srcx2 = 307;
                            s2.srcy2 = 48;
                            s2.wide = 305 * 2;
                            s2.high = (43) * 2;
                            s2.x = 2 * 2;
                            s2.y = 173 * 2;
                        }
                    }
                    if (s.mode == "word 6") {
                        auto & s2 = world.Sprite[1];
                        s2.length = 3;
                        s2.texture = 2; //1
                        s2.visible = true;
                        s2.srcx = 2;
                        s2.srcy = 51;
                        s2.srcx2 = 311;
                        s2.srcy2 = 71;
                        s2.wide = 309 * 2;
                        s2.high = (20) * 2;
                        //.x = 1: .y = 178
                    }
                    if (s.mode == "word 7") {
                        {
                            auto & s2 = world.Sprite[1];
                            s2.length = 4;
                            //s2.texture = 1;
                            s2.visible = true;
                            s2.srcx = 2;
                            s2.srcy = 75;
                            s2.srcx2 = 295;
                            s2.srcy2 = 117;
                            s2.wide = 293 * 2;
                            s2.high = (42) * 2;
                            //s2.x = 1: s2.y = 178;
                        } //end with
                    }
                    if (s.mode == "word 8") {
                        {
                            auto & s2 = world.Sprite[1];
                            s2.length = 6;
                            //s2.texture = 1;
                            s2.visible = true;
                            s2.srcx = 2;
                            s2.srcy = 120;
                            s2.srcx2 = 294;
                            s2.srcy2 = 185;
                            s2.wide = 292 * 2;
                            s2.high = (65) * 2;
                            //s2.x = 1: s2.y = 178;
                        } //end with
                    }
                    if (s.mode == "word 9") {
                        view.LoadTexture(-1, "Open3.png", 320, 240);
                        {
                            auto & s2 = world.Sprite[1];
                            //.texture = 1
                            s2.length = 6;
                            s2.visible = true;
                            s2.srcx = 2;
                            s2.srcy = 189;
                            s2.srcx2 = 305;
                            s2.srcy2 = 254;
                            s2.wide = 303 * 2;
                            s2.high = (65) * 2;
                            //.x = 1: .y = 178
                        } //end with
                    }
                    if (s.mode == "word 10") {
                        view.LoadTexture(-1, "Open4.png", 320, 240);
                        {
                            auto & s2 = world.Sprite[1];
                            s2.length = 6;
                            s2.texture = 3;
                            s2.visible = true;
                            s2.length = 7;
                            s2.srcx = 1;
                            s2.srcy = 4;
                            s2.srcx2 = 312;
                            s2.srcy2 = 69;
                            s2.wide = 313 * 2;
                            s2.high = (65) * 2;
                            //.x = 1: .y = 178
                        } //end with
                    }
                    if (s.mode == "word 11") {
                        view.LoadTexture(-1, "Open5.png", 320, 240);
                        {
                            auto & s2 = world.Sprite[1];
                            s2.length = 6;
                            //s2.texture = 1;
                            s2.visible = true;
                            s2.srcx = 1;
                            s2.srcy = 72;
                            s2.srcx2 = 258;
                            s2.srcy2 = 115;
                            s2.wide = 259 * 2;
                            s2.high = (43) * 2;
                            s2.x = 58 * 2;
                            s2.y = 188 * 2;
                        }
                    }
                    if (s.mode == "word 12") {
                        //if (s.miscTime < clock) then
                        //killS (1)
                        view.LoadTexture(-1, "PlainBlack.png", 320, 240);
                        s.mode = "word 13";
                        {
                            auto & s2 = world.Sprite[1];
                            s2.invTime = 10;
                            s2.length = 6;
                            //s2.miscTime = 0;
                            s2.mode = "words1";
                            s2.texture = 4;
                            s2.visible = true;
                            s2.srcx = 1;
                            s2.srcy = 1;
                            s2.srcx2 = 320;
                            s2.srcy2 = 240;
                            s2.wide = 320 * 2;
                            s2.high = (240) * 2;
                            s2.x = 1;
                            s2.y = 1;
                            world.Sprite[0].mode = "KILLDEATH";
                        }
                        //End If
                    }

                }
                if (world.Sprite[1].mode == "words2") {
                    //playWave "conzero.wav"
                    world.Sprite[1].mode = "words1";
                    s.miscTime = 0;
                    if (s.mode == "word 11") {
                        s.mode = "word 12"; //: .miscTime = clock + 3: Sprite[1].length = 0
                    }
                    if (s.mode == "word 10") {
                        s.mode = "word 11"; //: Sprite[1].length = 0
                    }
                    if (s.mode == "word 9") { s.mode = "word 10"; }
                    if (s.mode == "word 8") { s.mode = "word 9"; }
                    if (s.mode == "word 7") { s.mode = "word 8"; }
                    if (s.mode == "word 6") { s.mode = "word 7"; }
                    if (s.mode == "word 5") { s.mode = "word 6"; }
                    if (s.mode == "word 4") { s.mode = "word 5"; }
                    if (s.mode == "word 3") { s.mode = "word 4"; }
                    if (s.mode == "word 2") { s.mode = "word 3"; }
                    if (s.mode == "waka do") { s.mode = "word 2"; }
                }
            }

            //WORDS 1
            if (s.name == "words1") {

                if (s.mode == "words1") {
                    if (s.miscTime < 255) {
                        s.miscTime = s.miscTime + (world.sFactor * s.invTime);
                        //0s.3 * sFactor
                        s.color = to_rgb(s.miscTime, s.miscTime, s.miscTime);
                    }
                    if (s.miscTime > 245) {
                        //playWave "conten.wav"
                        s.color = view::qb_color(15);
                        s.mode = "words1b";
                        s.miscTime = world.clock + s.length;
                    }
                }

                if (world.Sprite[0].mode == "KILLDEATH" && s.mode == "words1b") {
                    s.name = "";
                }

                if (s.mode == "words1b" && s.miscTime < world.clock) {
                    s.mode = "words1c";
                    s.miscTime = 255;
                }

                if (s.mode == "words1c") {
                    if (s.miscTime > 0) {
                        s.miscTime = s.miscTime - (world.sFactor * 20); //0.3 * sFactor
                        if (s.miscTime > 0) {
                            s.color = to_rgb(s.miscTime, s.miscTime, s.miscTime);
                        }
                    }
                    if (s.miscTime < 1) {
                        //playWave "conten.wav"
                        s.color = view::qb_color(0);
                        s.mode = "words2";
                    }
                }
            }
        }


		if (world.screen == "Select Player") {
			world.screen = "SelectPlayerz";
			return create_select_screen(view, sound, vb, keys_pressed);
		}
		create_billboards(world, view.billboards());
		return nullptr;
    }


private:

    void findOrder() {
        int texorg = 0;
        int davidorg1 = 0;

        //2017: This is madness. Looks like it set `drawTrue` to false for
        //      everything, then went through and found the sprites with the
        //      highest z order first and put them in the drawOrder array.
        //      At the end the drawOrder array has indices of sprites from
        //      the close (high z order) ones to the far away (low z order)
        //      ones.
        for (int j = 0; j <= world.spritesInUse; ++ j) {
         world.Sprite[j].drawTrue = false;
         world.drawOrder[j] = -150;
        }

        for (int j = 0; j <= world.spritesInUse; ++ j) {
         texorg = -150;
         davidorg1 = 0;
         for (int k = 0; k <= world.spritesInUse; ++ k) {
             if (world.Sprite[k].zOrder > texorg
                 && world.Sprite[k].drawTrue == false) {
                 texorg = world.Sprite[k].zOrder;
                 davidorg1 = k;
             }
         }
         world.drawOrder[j] = davidorg1;
         world.Sprite[davidorg1].drawTrue = true;
        }

        //2017: Sanity check
        int last_value = 9999;
        for (int i = 0; i <= world.spritesInUse; ++i) {
            int next_value = world.Sprite[world.drawOrder[i]].zOrder;
            LP3_ASSERT(last_value >= next_value);
            last_value = next_value;
        }
    }


};  // end of GameImpl class


GameProcess * create_title_screen(view::View & view, Sound & sound, Vb & vb) {
	return new TitleScreenImpl(view, sound, vb);
}

}   }  // end namespace
