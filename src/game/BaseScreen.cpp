#include "BaseScreen.hpp"

namespace nnd3d { namespace game {

void create_billboards(World & world, std::vector<view::Billboard> & billboards) {
	billboards.clear();

	glm::vec2 res2d{ world.FULLSCREENWIDTH, world.FULLSCREENHEIGHT };

	// Do goofy animation stuff - maybe this should be moved
	for (int j = 0; j < world.spritesInUse; ++j) {
		auto & s = world.Sprite[j];
		if (s.frame > 0) {
			s.srcx = s.Aframe[s.frame].x;
			s.srcy = s.Aframe[s.frame].y;
			s.srcx2 = s.Aframe[s.frame].x2;
			s.srcy2 = s.Aframe[s.frame].y2;
		}
		if (s.reverse == true) {
			std::swap(s.srcx, s.srcx2);
		}
	}

	const glm::vec2 camera_pos = Camera(world.camera).ul();
	const glm::vec2 camera_size = Camera(world.camera).size();

	// Do the goofy camera billboard
	if (true) { // is this EVER disabled? textures[0]) {	// if not, consider this disabled
		view::Billboard b;
		b.ul = glm::vec2{ 0, 0 };
		b.size = res2d;
		b.tex_src_ul = camera_pos;
		b.tex_src_dr = camera_pos + camera_size;
		b.texture_index = 0;
		b.z = 0.9999f; // Underneath everything
		billboards.push_back(b);
	}

	for (int j = 0; j < world.spritesInUse; ++j) {
		auto & sprite = world.Sprite[j];
		if (!sprite.visible) {
			continue;
		}
		view::Billboard b;
		b.ul = glm::vec2{ sprite.x, sprite.y };
		b.size = glm::vec2{ sprite.wide, sprite.high };
		b.tex_src_ul = glm::vec2{ sprite.srcx, sprite.srcy };
		b.tex_src_dr = glm::vec2{ sprite.srcx2, sprite.srcy2 };
		b.texture_index = sprite.texture + 1;

		// Subtract camera
		b.ul -= camera_pos;

		// zOrder was used to manually define the layout. However it looks like
		// more reliably it was used as if things were on "layers" (think SNES).
		// So it should be possible to check every value these could have been.
		switch (sprite.zOrder) {
		case -149:	// Looks like the stage text
		case -100:	// Used by the clock / time shown at start
			b.z = 0.0001f;  // On top of everything
			break;
		case -99:	// When the player is hurt- I guess I want it to appear
					// over everything else.
			b.z = 0.0002f;  // on top of lots of stuff
			break;
		case -90:	// general player (?!!)
		case -1:	// bullets (??!)
		case 0:
			// This is the new smart way to determine this.
			// Use ground based y cordinate to determine z order (lower they are, the
			// likelier they are to cover up something)
			b.z = 1.0f - ((b.ul.y + b.size.y) / res2d.y);
			if (b.z < 0.0003f) {
				// Don't want anything to appear over the upper layers
				b.z = 0.0003f;
			}
			break;
		default:
			// Turns out the damn levels have random crap all over the place
			// with custom z orders. Almost 100% of the time the rule is "put
			// this behind everything" so I'll just do that.
			LP3_ASSERT(sprite.zOrder <= 10);
			b.z = 0.9998f - (0.0020f * ((10.0f / sprite.zOrder) + j / world.spritesInUse));
			//	d//efault:
			//	b.z = 1.0f - (100.0f / sprite.zOrder);
			//break;
		}


		// Now subtract sprite's z value, which is how high off the ground
		// they are.
		b.ul.y -= lp3::narrow<float>(sprite.z);
		billboards.push_back(b);
	}

}
void destroyEverything(World & world, view::View & view, Sound & sound, int how) {
	int penguin;
	int goatorg;

	//sound.PlayIsoWave("nothing.wav");

	if (how != 2) {
		sound.PlayBgm("");
	}

	view.LoadTexture(-1, "PlainBlack.png", 25, 25);
	world.camera.CameraX = 0;
	world.camera.CameraY = 0;
	goatorg = 0;
	penguin = world.spritesInUse;
	//If how = 2 Then goatorg = 30: penguin = 95

	for (int j = goatorg; j <= penguin; ++j) {
		world.Sprite[j] = CharacterSprite{};
	}
}

void flicker(World & world) {
	for (int j = 0; j < world.spritesInUse; ++j) {
		auto & s = world.Sprite[j];

		if (s.trueVisible != 0) {
			if (s.trueVisible == 1) { s.visible = true; }
			if (s.trueVisible == 2) { s.visible = false; }
			//s.trueVisible = 0
		}

		if (s.flickerTime > world.clock) {
			if (s.trueVisible == 0) {
				if (s.visible == false) { s.trueVisible = 2; }
				if (s.visible == true) { s.trueVisible = 1; }
			}

			if (s.flickOn == true) { s.visible = false; }
			if (s.flickOn == false) { s.visible = true; }
			if (s.flickOn == true) {
				s.flickOn = false;
			}
			else {
				s.flickOn = true;
			}
		}
	}
}

void findOrder(World & world) {
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

void set_time_stuff(World & world) {
	world.sFactor = speed_factor;
	world.clock += 0.016f;
	// TODO: lemon time is ignored, for now
	// used to affect sFactory, but not the clock
}

}   }
