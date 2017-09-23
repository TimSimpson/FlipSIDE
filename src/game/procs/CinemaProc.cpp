#include "CinemaProc.hpp"

namespace nnd3d { namespace game { namespace proc {

namespace {
	void centerSprite(CharacterSprite & s, Camera & camera) {
		// Aims the camera on the given sprite.
		s.x = (camera.x() + (camera.width() / 2)) - (s.wide / 2);
		s.y = (camera.y() + (camera.height() / 2)) - (s.high / 2);
	}
}

class CinemaProc : public CharacterProc {
private:
	int counter;
    CharacterProcEnv env;
    EntityManager & e_manager;
public:
    std::vector<CinemaType> cinema;
private:
    gsl::span<CharacterSprite> sprites;


    // This is converted stuff from my old-school string based state machines
    // which is why it's confusing.
    enum class Segments {
        clock,
        faces
    };

    Segments segment;
    std::string mode;

    double misc_time;

public:
    CinemaProc(CharacterProcEnv _env, EntityManager & _e_manager)
    :   counter(0),
        env(_env),
        e_manager(_e_manager),
        cinema(),
        sprites(e_manager.grab_sprites(4)),
        segment(Segments::clock),
        mode(),
        misc_time(0)
    {
        sprites[2].name = "cinema"; sprites[2].zOrder = -149;
        sprites[1].name = "cinema"; sprites[1].zOrder = -149;
        sprites[0].name = "cinema"; sprites[0].zOrder = -149;
        sprites[3].name = "cinema"; sprites[3].zOrder = -149;
        //world.Sprite[30].zOrder = -149;

        mode = "";
    }

    void animate(std::int64_t ms) override {

    }

    void death_animation() override {
        throw std::logic_error("The method or operation is not implemented.");
    }

    void load_graphics(const std::string & texture_file,
                       const std::string & animation_file)
    {
		env.context.view.LoadTexture(9, texture_file, 400, 400);

         for (auto & s : sprites) {
            env.context.view.load_animation_file(s.Aframe, animation_file);
        }
    }
    void initialize() override {
        throw std::logic_error("The method or operation is not implemented.");
    }

    void set_cinema(const std::vector<CinemaType> & new_cinema) {
        this->cinema = new_cinema;
        this->counter = 0;
    }

	void skip_to_talking() {
		this->segment = Segments::faces;
	}

    CharacterProc * spawn(CharacterSprite & sprite, const std::string & name) override {
        throw std::logic_error("The method or operation is not implemented.");
    }

    void clock_animation() {
        if (mode == "") {
            //playWave "conten.wav"
            if (misc_time == 0) {
                misc_time = env.current_time + 2;
                sprites[0].zOrder = -100;
            }

            {
                //TSNOW: No idea why this is hardcoded.
                auto & s2 = sprites[0];
                if (misc_time > env.current_time) {
                    s2.x = env.camera.x();
                    s2.y = env.camera.y();
                    s2.wide = 640;
                    s2.high = 480;
                    s2.frame = 9;
                    s2.texture = 9;
                    s2.visible = true;
                }

                if (misc_time < env.current_time) {
                    s2.x = s2.x + 4 * speed_factor;
                    s2.y = s2.y + 4 * speed_factor;
                    s2.wide = s2.wide - 8 * speed_factor;
                    s2.high = s2.high - 8 * speed_factor;
                    s2.zOrder = -100;
                    if (s2.wide < 0) {
                        mode = "?";
                        misc_time = env.current_time + 3;
                    }
                }
            }  // End With
        }  //mode
        if (mode == "?") {
            //playWave "conten.wav"
            {
                auto & s2 = sprites[0];
                unstretch(s2);
                s2.wide = s2.wide * 2;
                s2.high = s2.high * 2;

                //.wide = (275 - 188) * 2
                //.high = (376 - 353) * 2

                s2.frame = 10;
                s2.texture = 9;
                s2.visible = true;
                s2.zOrder = -100;
                centerSprite(sprites[0], env.camera);
            }
            if (misc_time < env.current_time) {
                mode = "2";
                segment = Segments::faces;
            }
        }
    }

    void GoSub_setUpLevel1() {
        for (auto & s : sprites) {
            s.visible = true;
            s.wide = (271 - 137) * 2;
            s.high = 81 * 2;
            s.seekx = 268;
            s.seeky = 1;
            s.texture = 9;
        }
    }


    void GoSub_level1a() {
        sprites[0].frame = cinema[counter].frame1;
        sprites[1].frame = cinema[counter].frame2;
        sprites[2].frame = cinema[counter].frame3;
        sprites[3].frame = cinema[counter].frame4;
        sprites[0].color = view::qb_color(cinema[counter].color1);
        sprites[1].color = view::qb_color(cinema[counter].color2);
        sprites[2].color = view::qb_color(cinema[counter].color3);
        sprites[3].color = view::qb_color(cinema[counter].color4);

        env.context.sound.PlayIsoWave(cinema[counter].wavefile);
        misc_time = env.current_time + cinema[counter].miscTime;
        counter = counter + 1;
    }

    void GoSub_level1b() {
        for (auto & s : sprites) {
            s.visible = true;
            s.wide = (271 - 137) * 2;
            s.high = 81 * 2;
            s.seekx = 0;
            s.seeky = 1;
            s.texture = 9;
        }
    }

    void GoSub_level1c() {
        sprites[0].x = env.camera.x() - sprites[0].seekx;
        sprites[0].y = env.camera.y() + 20;
        sprites[1].x = env.camera.x() + env.camera.width() - 268 + sprites[1].seekx;
        sprites[1].y = env.camera.y() + 20;
        sprites[2].x = env.camera.x() - sprites[2].seekx;
        sprites[2].y = env.camera.y() + env.camera.height() - 180;
        sprites[3].x = env.camera.x() + env.camera.width() - 268 + sprites[2].seekx;
        sprites[3].y = env.camera.y() + env.camera.height() - 180;

        for (auto & s : sprites) {
            // If i = 34 Then GoTo kiddy2
            s.seekx = lp3::narrow<int>(s.seekx + speed_factor * 6);
            s.seeky = s.seeky + 1;
            if (s.seeky == 3) {
                s.seeky = 1;
            }
            s.frame = s.seeky;
        }

        if (sprites[1].seekx > 268) {
            for (auto & s : sprites) {
                s.visible = false;
            }

            mode = "9";
        }
    }


    void face_animation() {
        if (mode != "3" && mode != "2") {
            // move all the faces so that they line up
            sprites[0].x = env.camera.x(); //  - sprites[0].seekx
            sprites[0].y = env.camera.y() + 20;
            sprites[1].x = env.camera.x() + env.camera.width() - 268; // + sprites[1].seekx
            sprites[1].y = env.camera.y() + 20;
            sprites[2].x = env.camera.x(); //  - sprites[2].seekx
            sprites[2].y = env.camera.y() + env.camera.height() - 180;
            sprites[3].x = env.camera.x() + env.camera.width() - 268; //  + sprites[2].seekx
            sprites[3].y = env.camera.y() + env.camera.height() - 180;
        }

        if (mode == "6") {
            GoSub_level1c();
        }

        if (mode == "5") {
            GoSub_level1b();
            mode = "6";
        }
        if (mode == "4" && misc_time < env.current_time) {
            if (counter >= cinema.size()) {
                mode = "5";
            } else {
                GoSub_level1a();
            }
        // mode = "5"
        }
        if (mode == "2") {
            if (counter > cinema.size()) {
                mode = "9";
                sprites[0].visible = false;
                return;
            }
            GoSub_setUpLevel1();
            mode = "3";
        }
        if (mode == "3") {
            sprites[0].x = env.camera.x() - sprites[0].seekx;
            sprites[0].y = env.camera.y() + 20;
            sprites[1].x = env.camera.x() + env.camera.width() - 268 + sprites[1].seekx;
            sprites[1].y = env.camera.y() + 20;
            sprites[2].x = env.camera.x() - sprites[2].seekx;
            sprites[2].y = env.camera.y() + env.camera.height() - 180;
            sprites[3].x = env.camera.x() + env.camera.width() - 268 + sprites[2].seekx;
            sprites[3].y = env.camera.y() + env.camera.height() - 180;

            if (sprites[0].seekx < 0) {
                mode = "4";
                sprites[0].seekx = 0;
                sprites[1].seekx = 0;
                sprites[2].seekx = 0;
                sprites[3].seekx = 0;
            }

            for (auto & sprite : sprites)  {
                sprite.seekx = lp3::narrow<int>(sprite.seekx - speed_factor * 6);
                sprite.frame = sprite.frame + 1;
                if (sprite.frame > 2) {
                    sprite.frame = 1;
                }
                // Sprite(i).frame = Sprite(i).seeky
            }
        }
    }

    bool update() override {
        if (Segments::clock == segment) {
            clock_animation();
        } else {
            face_animation();
        }
		return true;
    }

};


void set_level(CinemaProc & proc, const float stage) {
    // Level 1, door
    if (1.0 == stage || 1.1 == stage) {
        proc.load_graphics("Level1Cinema.png", "Level1Cinema.ani");
		std::vector<CinemaType> cinema = {
			CinemaType(5, 3, 7, 16, 15, 8, 8, 8, "TalkLv1A1.wav", 4.49),
			CinemaType(5, 3, 7, 16, 8, 15, 8, 8, "TalkLv1A2.wav", 1.87)
		};
        proc.set_cinema(cinema);
    } else if (1.1 <= stage && 1.2 > stage) {
        proc.load_graphics("Level1Cinema.png", "Level1Cinema.ani");
		proc.skip_to_talking();
        proc.set_cinema(std::vector<CinemaType>{
            {5, 3, 7, 16, 8, 8, 15, 8, "TalkLv1B1.wav", 2.24},
            {5, 3, 7, 16, 8, 15, 8, 8, "TalkLv1B2.wav", 2.74},
            {5, 3, 7, 16, 15, 8, 8, 8, "TalkLv1B3.wav", 4.24},
            {5, 3, 7, 16, 15, 15, 15, 8, "TalkLv1B4.wav", 0.95}
        });
    } else if (1.2 <= stage && 1.25 > stage) {
        proc.load_graphics("Level1BCinema.png", "Level1bCinema.ani");
        proc.set_cinema(std::vector<CinemaType>{
            {5, 3, 7, 16, 8, 8, 15, 8, "TalkLv1C1.wav", 3.31},
            {5, 3, 7, 16, 8, 15, 8, 8, "TalkLv1C2.wav", 2.3},
            {5, 3, 7, 16, 15, 8, 8, 8, "TalkLv1C3.wav", 3.05}
        });
    } else if (1.3 == stage) {
        // proc.load_graphics("Level1BCinema.png", "Level1bCinema.ani");
        // world.cinemaMax = 3;
        // world.counter = 4;
        // proc.set_cinema(std::vector<CinemaType>{
        //     {7, 7, 7, 7, 8, 8, 15, 8, "TalkLv1c1.wav", 3.31},
        //     {7, 7, 7, 7, 8, 15, 8, 8, "TalkLv1c2.wav", 2.3},
        //     {7, 7, 7, 7, 15, 8, 8, 8, "TalkLv1c3.wav", 3.05}
        // });
    } else if (1.4 == stage) {
        // proc.load_graphics("Level1DCinema.png", "Level1bCinema.ani");
        // world.cinemaMax = 3;
        // world.counter = 4;
        // proc.set_cinema(std::vector<CinemaType>{
        //     {7, 7, 7, 7, 8, 8, 15, 8, "TalkLv1c1.wav", 3.31},
        //     {7, 7, 7, 7, 8, 15, 8, 8, "TalkLv1c2.wav", 2.3},
        //     {7, 7, 7, 7, 15, 8, 8, 8, "TalkLv1c3.wav", 3.05},
        // });
    }
}

CharacterProc * create_cinema_proc(
    CharacterProcEnv env, EntityManager & e_manager,
    const float stage) {
    std::unique_ptr<CinemaProc> proc(new CinemaProc(env, e_manager));
    set_level(*proc, stage);
    return proc.release();
}

}   }   }
