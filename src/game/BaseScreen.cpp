#include "BaseScreen.hpp"

namespace nnd3d { namespace game {

void create_billboards(World & world, std::vector<view::Billboard> & billboards) {
    billboards.clear();

    glm::vec2 res2d{ world.FULLSCREENWIDTH, world.FULLSCREENHEIGHT };

    // Do goofy animation stuff - maybe this should be moved
    for (std::size_t j = 0; j < world.Sprite.size(); ++j) {
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
    if (true) { // is this EVER disabled? textures[0]) {    // if not, consider this disabled
        view::Billboard b;
        b.ul = glm::vec2{ 0, 0 };
        b.size = res2d;
        b.tex_src_ul = camera_pos;
        b.tex_src_dr = camera_pos + camera_size;
        b.texture_index = 0;
        b.z = 0.9999f; // Underneath everything
        billboards.push_back(b);
    }

    for (std::size_t j = 0; j < world.Sprite.size(); ++j) {
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
        case -149:  // Looks like the stage text
        case -100:  // Used by the clock / time shown at start
            b.z = 0.0001f;  // On top of everything
            break;
        case -99:   // When the player is hurt- I guess I want it to appear
                    // over everything else.
            b.z = 0.0002f;  // on top of lots of stuff
            break;
        case -90:   // general player (?!!)
        case -1:    // bullets (??!)
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
            b.z = 0.9998f - (0.0020f * ((10.0f / sprite.zOrder) + j / world.Sprite.size()));
            //  d//efault:
            //  b.z = 1.0f - (100.0f / sprite.zOrder);
            //break;
        }


        // Now subtract sprite's z value, which is how high off the ground
        // they are.
        b.ul.y -= lp3::narrow<float>(sprite.z);

        b.color = sprite.color;
        billboards.push_back(b);
    }

}
void destroyEverything(World & world, view::View & view, Sound & sound, int how) {
    //sound.PlayIsoWave("nothing.wav");

    if (how != 2) {
        sound.PlayBgm("");
    }

    view.LoadTexture(-1, "PlainBlack.png", 25, 25);

    world.camera.focus({0, 0});

    //If how = 2 Then goatorg = 30: penguin = 95

    for (auto & s : world.Sprite) {
        s = CharacterSprite{};
    }
}

void flicker(World & world) {
    for (std::size_t j = 0; j < world.Sprite.size(); ++j) {
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

void set_time_stuff(World & world) {
    world.sFactor = speed_factor;
    world.clock += 0.016f;
    // TODO: lemon time is ignored, for now
    // used to affect sFactory, but not the clock
}

}   }
