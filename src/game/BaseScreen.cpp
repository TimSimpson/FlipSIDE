#include "BaseScreen.hpp"

namespace nnd3d { namespace game {

void destroyEverything(World & world, view::View & view, Sound & sound, int how) {
	int penguin;
	int goatorg;	

	//sound.PlayIsoWave("nothing.wav");

	if (how != 2) {
		sound.PlayBgm("");
	}

	view.LoadTexture(-1, "PlainBlack.png", 25, 25);
	world.gotFocus = -1;
	world.CameraX = 0;
	world.CameraY = 0;
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


}   }
