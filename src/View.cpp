#include "View.hpp"
#include "World.hpp"

// The old VB code constantly was tossing ints into floats and vice-versa,
// and using lp3::narrow everywhere
#pragma warning(disable: 4244)

namespace nnd3d {

namespace {
	constexpr int letters_max = 80 * 24;
	const glm::ivec4 normColor{ 255, 255, 255, 255 };
	const glm::ivec2 res2d(World::FULLSCREENWIDTH, World::FULLSCREENHEIGHT);
}

View::View(core::MediaManager & _media, World & _world)
:	media(_media),
	bgtexture(),
	AnimationTexture({}),
	program(),
	font{ media.load("../Engine/apple_kid.fnt") },
	font_elements{ (letters_max * 4) },
	font_quads(font_elements.add_quads(letters_max)),
	game_elements(),
	world(_world),
	tex_size(),
	bgverts({}),
	bg_size()
{
	// Create one vertex buffer for the bgtexture
	game_elements.emplace_back(letters_max * 4);
	// Then others for each other texture. Make each one the maximum size it
	// might need to be to represent all sprites on the screen, even though
	// this is wasteful.
	for (std::size_t i = 0; i < AnimationTexture.size(); ++i) {
		game_elements.emplace_back(World::NUMSPRITES * 4);
	}
}

void View::operator()(const glm::mat4 & previous) {
	program.use();
	auto _2d = gfx::create_2d_screen(previous, res2d);
	program.set_mvp(_2d);

	for (int i = 0; i < 10; ++ i) {
		lp3::gfx::Texture * tex;
		if (i == 0) {
			tex = bgtexture.get();
		} else {
			tex = AnimationTexture[i - 1].get();
		}
		if (tex && 0 < game_elements[i].count()) {
			program.set_texture(tex->gl_id());
			program.draw(game_elements[i]);
		}
	}

	program.set_texture(font.texture().gl_id());
	program.draw(font_elements);
}

void View::DrawStuff(float fps) {
	// 2017- draw FPS text

	// Clear theses
	for (auto & ge : game_elements) {
		ge.reset();
	}

	std::string s = str(boost::format("FPS: %d") % fps);
	gfx::write_string(font_quads, font, glm::vec2(520, 10), 0.0f, 40.0f, s);

	if (bgtexture) {
		LP3_ASSERT(nullptr != bgtexture);
		draw_verts_as_quad(bgverts.data(), -1);
	}
	int currentTexture = -1;
	for (int j = 0; j < world.spritesInUse; ++j) {
		if (world.drawOrder[j] == -1) {
			continue;   // goto suckIt
		}
		const int index = world.drawOrder[j];
		const auto & sprite = world.Sprite[index];


		if (sprite.visible) {
			draw_verts_as_quad(sprite.SpriteVerts.data(), sprite.texture);
		}
	}
	// If ((GetTickCount / 1000) > cranBerry) Then cranBerry = ((GetTickCount / 1000) + 1): frRate = frRate2: frRate2 = 0
	//If debugOn = True Then Form1.PSet (1, 1): Form1.Print "FPS:" + Str$(frRate): Form1.Print "GPS:" + Str$(gpRate)
}

void View::draw_verts_as_quad(const Vertex * v, const int texIndex) {
	LP3_ASSERT(texIndex >= -1);
	LP3_ASSERT(texIndex < lp3::narrow<int>(AnimationTexture.size()));

	if (texIndex > -1) {
		int five = 5;
	}
	// Using old game logic, -1 could be the "background" texture which was
	// reasonlessly treated differently from everything else.
	const int realIndex = texIndex + 1;
	gfx::Quad<gfx::TexVert> quad = game_elements[realIndex].add_quad();
	draw_vert_to_quad(v, quad);
}

void View::ForceShowBackground() {
	// This was originally a block of code in the "NowLoading" function.
        // It appears it would force the background to display, pre-empting
        // the loop, so that while busy work took place at least the loading
        // screen would be visible.
        /*if (Form1.WindowState != vbMinimized) {

            //cranBerry = Timer

            With dev
                Call .Clear(0, ByVal 0&, D3DCLEAR_TARGET, &HFF, 0, 0)

                Call .BeginScene



                'Call .CopyRects(bgSurface, 1, 1, .GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO), 1)


                Call .SetTexture(0, bgtexture)
                 Call .DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, bgverts(0), Len(bgverts(0)))
                .EndScene
                Call .Present(ByVal 0&, ByVal 0&, 0, ByVal 0&)
            End With
        }*/
}

gsl::owner<gfx::Texture *> View::load_image(const std::string & fileName) {
    // Yes, this old game was so silly it used black as its color key. :/
    LP3_LOG_DEBUG("Loading texture %s", fileName);
    auto bmp_file = media.path(fileName);
    glm::ivec3 color_key[] = { glm::ivec3{ 0, 0, 0 }};
    return new gfx::Texture(
        IMG_Load(bmp_file.c_str()),
        gsl::make_span(color_key, 1));
}

void View::LoadTexture(int which, const std::string & fileName, int howWide,
                       int howHigh) {
	LP3_LOG_DEBUG("LoadTexture %i %s %i %i", which, fileName, howWide, howHigh);
    if (which == -1) {
        bgtexture.reset(load_image(fileName));
		bg_size = bgtexture->size();
		LP3_LOG_DEBUG("   size = %i, %i", bg_size.x, bg_size.y);
    } else {
        AnimationTexture[which].reset(load_image(fileName));
		tex_size[which] = AnimationTexture[which]->size();
		LP3_LOG_DEBUG("   size = %i, %i", tex_size[which].x, tex_size[which].y);
    }
}

glm::ivec4 View::Rgb(int r, int g, int b) const {
	// Emulates old VB RGB function. Named 'Rgb' due to probable windows.h
	// issues. >:(
	const glm::ivec4 rgb(r, g, b, 255);
	LP3_ASSERT(rgb.r == r && rgb.g == g && rgb.b == b);
	return rgb;
}

glm::ivec4 View::QBColor(int index) {
	// If the code picks wrong, they get translucent yellow.
    switch(index) {
        case 0:
            return glm::ivec4{0, 0, 0, 255};
        case 1:
            return glm::ivec4{0, 0, 128, 255};
        case 2:
            return glm::ivec4{0, 128, 0, 255};
        case 3:
            return glm::ivec4{128, 255, 128, 255};
        case 4:
            return glm::ivec4{128, 0, 0, 255};
        case 5:
            return glm::ivec4{128, 0, 128, 255};
        case 6:
            return glm::ivec4{128, 128, 0, 255};
        case 7:
            return glm::ivec4{192, 192, 192, 255};
        case 8:
            return glm::ivec4{64, 64, 64, 255};
        case 9:
            return glm::ivec4{0, 0, 255, 255};
        case 10:
            return glm::ivec4{0, 255, 0, 255};
        case 11:
            return glm::ivec4{128, 128, 255, 255};
        case 12:
            return glm::ivec4{255, 0, 0, 255};
        case 13:
            return glm::ivec4{255, 0, 255, 255};
        case 14:
            return glm::ivec4{255, 255, 0, 255};
        case 15:
            return glm::ivec4{255, 255, 255, 255};
        default:
			LP3_ASSERT(false);  // Bad color selection.
            return glm::ivec4{255, 255, 0, 128};
    }
}

int View::texWidth(int index) {
	if (index < 0) {
		return this->bg_size.x;
	}
	return this->tex_size[index].x;
}

int View::texHeight(int index) {
	if (index < 0) {
		return this->bg_size.y;
	}
	return this->tex_size[index].y;
}

void View::UpdateSprites() {
     //int j = 0; // in old code, not needed?
    int k = 0;
    double texorg = 0.0;
    int davidorg1 = 0;
    int davidOrg2 = 0;

    for (int j = 0; j < world.spritesInUse; ++ j) {
        auto & s = world.Sprite[j];
        if (s.frame > 0) {
            s.srcx = s.Aframe[s.frame].x;
            s.srcy = s.Aframe[s.frame].y;
            s.srcx2 = s.Aframe[s.frame].x2;
            s.srcy2 = s.Aframe[s.frame].y2;
        }
    }

    //----------------------------------------------------------------------
    //              THIS PART HERE'S THE KICKER
    //----------------------------------------------------------------------

    {
        auto & v = this->bgverts[0];
        v.x = 0; v.y = 480; // RealHeight
   //      v.tu = lp3::narrow<float>(world.CameraX) / lp3::narrow<float>(world.bgWidth);
   //      v.tv = (lp3::narrow<float>(world.CameraY) + lp3::narrow<float>(world.CameraHeight))
			// / lp3::narrow<float>(world.bgHeight);
        v.tu = lp3::narrow<float>(world.CameraX) / lp3::narrow<float>(bg_size.x);
        v.tv = (lp3::narrow<float>(world.CameraY) + lp3::narrow<float>(world.CameraHeight))
            / lp3::narrow<float>(bg_size.y);
        v.rhw = 1;
        v.color = normColor;
    }

    {
        auto & v = this->bgverts[1];
        v.x = 0; v.y = 0;
        v.tu = lp3::narrow<float>(world.CameraX) /
					lp3::narrow<float>(bg_size.x);
		v.tv = lp3::narrow<float>(world.CameraY) / lp3::narrow<float>(bg_size.y);
        v.rhw = 1;
        v.color = normColor;
    }

    {
        auto & v = this->bgverts[2];
        v.x = 640; v.y = 480; // RealWidth; v.y = RealHeight
        v.tu = lp3::narrow<float>(world.CameraX + world.CameraWidth) / bg_size.x;
        v.tv = lp3::narrow<float>(world.CameraY + world.CameraHeight) / bg_size.y;
        v.rhw = 1;
        v.color = normColor;
    }

    {
        auto & v = this->bgverts[3];
        v.x = 640; v.y = 0;
        v.tu = lp3::narrow<float>(world.CameraX + world.CameraWidth)
				/ bg_size.x;
		v.tv = lp3::narrow<float>(world.CameraY) / bg_size.y;
        v.rhw = 1;
        v.color = normColor;
    }

    for (int j = 0; j < world.spritesInUse; ++ j) {
        auto & sprite = world.Sprite[j];

        if (sprite.reverse == true) {
            k = sprite.srcx2;
            sprite.srcx2 = sprite.srcx;
            sprite.srcx = k;
        }

        {
            auto & v = sprite.SpriteVerts[0];
            v.x = lp3::narrow<float>(sprite.x - world.CameraX);
            v.y = lp3::narrow<float>(
				sprite.y + sprite.high - (sprite.z) - world.CameraY);
            if (sprite.srcx != 0) {
                v.tu = (float) sprite.srcx / this->texWidth(sprite.texture);
            }
            if (sprite.srcy2 != 0) {
                v.tv = (float) sprite.srcy2 / this->texHeight(sprite.texture);
            }
            v.color = sprite.color;
        }
        {
            auto & v = sprite.SpriteVerts[1];
            v.x = sprite.x - world.CameraX;
            v.y = sprite.y - (sprite.z) - world.CameraY;
            if (sprite.srcx != 0) {
                v.tu = (float) sprite.srcx / this->texWidth(sprite.texture);
            }
            if (sprite.srcy != 0) {
                v.tv = (float) sprite.srcy / this->texHeight(sprite.texture);
            }
            // v.rhw = 1
            v.color = sprite.color;
        }
        {
            auto & v = sprite.SpriteVerts[2];
            v.x = sprite.x + sprite.wide - world.CameraX;
            v.y = sprite.y + sprite.high - (sprite.z) - world.CameraY;
            if (sprite.srcx2 != 0) {
                v.tu = (float) sprite.srcx2 / this->texWidth(sprite.texture);
            }
            if (sprite.srcy2 != 0) {
                v.tv = (float) sprite.srcy2 / this->texHeight(sprite.texture);
            }
            // v.rhw = 1
            v.color = sprite.color;
        }
        {
            auto & v = sprite.SpriteVerts[3];
            v.x = sprite.x + sprite.wide - world.CameraX;
            v.y = sprite.y - (sprite.z) - world.CameraY;
            if (sprite.srcx2 != 0) {
                v.tu = (float) sprite.srcx2 / this->texWidth(sprite.texture);
            }
            if (sprite.srcy != 0) {
                v.tv = (float) sprite.srcy / this->texHeight(sprite.texture);
            }
            // v.rhw = 1
            v.color = sprite.color;
        }
    }  // end for loop
}


}   // end namespace
