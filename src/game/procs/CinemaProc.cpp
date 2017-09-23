#include "CinemaProc.hpp"
#include <lp3/sims.hpp>

namespace nnd3d { namespace game { namespace proc {

namespace {
	void centerSprite(CharacterSprite & s, Camera & camera) {
		// Aims the camera on the given sprite.
		s.x = (camera.x() + (camera.width() / 2)) - (s.wide / 2);
		s.y = (camera.y() + (camera.height() / 2)) - (s.high / 2);
	}
}

class ClockAnimation : public CharacterProc {
private:
	Camera & camera;
	lp3::sims::CoroutineState state;
	CharacterSprite & s2;
	std::int64_t time;
	std::int64_t misc_time;
public:
	ClockAnimation(CharacterProcEnv _env, CharacterSprite & _s2, 
		           const std::string & animation_file)
	:	camera(_env.camera),
		state(),
		s2(_s2),
		time(0),
		misc_time(0)
	{
		_env.context.view.load_animation_file(s2.Aframe, animation_file);
	}

	bool update() override {
		time += ms_per_update;

		LP3_COROUTINE_BEGIN(state);
		misc_time = 2000;
		s2.zOrder = -100;

		while (misc_time > time) {
			s2.x = camera.x();
			s2.y = camera.y();
			s2.wide = 640;
			s2.high = 480;
			s2.frame = 9;
			s2.texture = 9;
			s2.visible = true;
			LP3_YIELD(true);
		}
		while (misc_time >=time) {
			LP3_YIELD(true);
		}
		while (s2.wide >= 0) {
			s2.x = s2.x + 4 * speed_factor;
			s2.y = s2.y + 4 * speed_factor;
			s2.wide = s2.wide - 8 * speed_factor;
			s2.high = s2.high - 8 * speed_factor;
			s2.zOrder = -100;
			LP3_YIELD(true);
		}

		misc_time = time + 3000;

		while (misc_time >= time) {
			unstretch(s2);
			s2.wide = s2.wide * 2;
			s2.high = s2.high * 2;

			//.wide = (275 - 188) * 2
			//.high = (376 - 353) * 2

			s2.frame = 10;
			s2.texture = 9;
			s2.visible = true;
			s2.zOrder = -100;
			centerSprite(s2, camera);
			LP3_YIELD(true);
		}

		s2.visible = false;
		LP3_COROUTINE_END()
		return false;
	}
};


class FaceRadio : public CharacterProc {
private:
	int counter;
	CharacterProcEnv env;
	std::vector<CinemaType> cinema;
	gsl::span<CharacterSprite> sprites;
	double misc_time;
	lp3::sims::CoroutineState state;

public:
	FaceRadio(CharacterProcEnv _env, EntityManager & e_manager,
		      const std::string & texture_file,
		      const std::string & animation_file,
			  const std::vector<CinemaType> & _cinema)
	:	counter(0),
		env(_env),
		cinema(_cinema),
		sprites(e_manager.grab_sprites(4)),
		misc_time(0)
	{
		env.context.view.LoadTexture(9, texture_file, 400, 400);

		for (auto & s : sprites) {
			s.name = "cinema";
			s.zOrder = -149;
			env.context.view.load_animation_file(s.Aframe, animation_file);
		}
	}

	bool update() override {
		// This function keeps the faces aligned as the player moves the camera
		auto line_up_faces = [&]() {
			// Make certain faces ended up in right spot:
			sprites[0].x = env.camera.x(); //  - sprites[0].seekx
			sprites[0].y = env.camera.y() + 20;
			sprites[1].x = env.camera.x() + env.camera.width() - 268; // + sprites[1].seekx
			sprites[1].y = env.camera.y() + 20;
			sprites[2].x = env.camera.x(); //  - sprites[2].seekx
			sprites[2].y = env.camera.y() + env.camera.height() - 180;
			sprites[3].x = env.camera.x() + env.camera.width() - 268; //  + sprites[2].seekx
			sprites[3].y = env.camera.y() + env.camera.height() - 180;
		};

		LP3_COROUTINE_BEGIN(state)
			for (auto & s : sprites) {
				s.visible = true;
				s.wide = (271 - 137) * 2;
				s.high = 81 * 2;
				s.seekx = 268;
				s.seeky = 1;
				s.texture = 9;
			}


		// Faces are sliding in
		while (true) {
			sprites[0].x = env.camera.x() - sprites[0].seekx;
			sprites[0].y = env.camera.y() + 20;
			sprites[1].x = env.camera.x() + env.camera.width() - 268 + sprites[1].seekx;
			sprites[1].y = env.camera.y() + 20;
			sprites[2].x = env.camera.x() - sprites[2].seekx;
			sprites[2].y = env.camera.y() + env.camera.height() - 180;
			sprites[3].x = env.camera.x() + env.camera.width() - 268 + sprites[2].seekx;
			sprites[3].y = env.camera.y() + env.camera.height() - 180;

			if (sprites[0].seekx < 0) {
				sprites[0].seekx = 0;
				sprites[1].seekx = 0;
				sprites[2].seekx = 0;
				sprites[3].seekx = 0;
				break;
			}

			for (auto & sprite : sprites) {
				sprite.seekx = lp3::narrow<int>(sprite.seekx - speed_factor * 6);
				sprite.frame = sprite.frame + 1;
				if (sprite.frame > 2) {
					sprite.frame = 1;
				}
				// Sprite(i).frame = Sprite(i).seeky
			}
			LP3_YIELD(true);
		}

		for (counter = 0; counter < cinema.size(); ++counter) {
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

			// Wait for the line to be delivered
			while (misc_time >= env.current_time) {
				line_up_faces();
				LP3_YIELD(true);
			}
		}

		line_up_faces();
		LP3_YIELD(true);

		for (auto & s : sprites) {
			s.visible = true;
			s.wide = (271 - 137) * 2;
			s.high = 81 * 2;
			s.seekx = 0;
			s.seeky = 1;
			s.texture = 9;
		}
		line_up_faces();
		LP3_YIELD(true);

		// Faces slide away
		while (sprites[1].seekx <= 268) {
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
			LP3_YIELD(true);
		}

		for (auto & s : sprites) {
			s.visible = false;
		}

		LP3_COROUTINE_END();
		return false;
	}
};

class ClockAndFaceCinematic : public CharacterProc {
private:
	ClockAnimation clock;
	FaceRadio faces;
	lp3::sims::CoroutineState state;

public:
	ClockAndFaceCinematic(CharacterProcEnv _env, EntityManager & e_manager,
						  const std::string & texture_file,
		                  const std::string & animation_file,
		                  const std::vector<CinemaType> & _cinema)
	:	clock(_env, e_manager.grab_sprite(), animation_file), 
		faces(_env, e_manager, texture_file, animation_file, _cinema),
		state()
	{}

	bool update() override {
		LP3_COROUTINE_BEGIN(state);
		while (clock.update()) { LP3_YIELD(true); }
		while (faces.update()) { LP3_YIELD(true); }
		LP3_COROUTINE_END();
		return false;
	}

};


CharacterProc * create_cinema_proc(
    CharacterProcEnv env, EntityManager & e_manager,
    const float stage) 
{
	if (1.0 == stage || 1.1 == stage) {
		std::vector<CinemaType> cinema = {
			CinemaType(5, 3, 7, 16, 15, 8, 8, 8, "TalkLv1A1.wav", 4.49),
			CinemaType(5, 3, 7, 16, 8, 15, 8, 8, "TalkLv1A2.wav", 1.87)
		};
		return new ClockAndFaceCinematic(
			env, e_manager, "Level1Cinema.png", "Level1Cinema.ani", cinema);		
	} else if (1.1 <= stage && 1.2 > stage) {
		return new FaceRadio(
			env, e_manager, "Level1Cinema.png", "Level1Cinema.ani",
			std::vector<CinemaType>{
				{5, 3, 7, 16, 8, 8, 15, 8, "TalkLv1B1.wav", 2.24},
				{ 5, 3, 7, 16, 8, 15, 8, 8, "TalkLv1B2.wav", 2.74 },
				{ 5, 3, 7, 16, 15, 8, 8, 8, "TalkLv1B3.wav", 4.24 },
				{ 5, 3, 7, 16, 15, 15, 15, 8, "TalkLv1B4.wav", 0.95 }
		});
	} else if (1.2 <= stage && 1.25 > stage) {
		return new ClockAndFaceCinematic(
			env, e_manager,
			"Level1BCinema.png", "Level1bCinema.ani",
			std::vector<CinemaType>{
				{5, 3, 7, 16, 8, 8, 15, 8, "TalkLv1C1.wav", 3.31},
				{ 5, 3, 7, 16, 8, 15, 8, 8, "TalkLv1C2.wav", 2.3 },
				{ 5, 3, 7, 16, 15, 8, 8, 8, "TalkLv1C3.wav", 3.05 }
			});
	} else if (1.3 == stage) {
		return new ClockAnimation(
			env, e_manager.grab_sprite(), "Level1bCinema.ani");
	} else if (1.4 == stage) {
		return new ClockAnimation(
			env, e_manager.grab_sprite(), "Level1bCinema.ani");
	}
	return nullptr;
}

}   }   }
