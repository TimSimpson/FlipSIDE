#include "CharacterSprite.hpp"

namespace nnd3d {

CharacterSprite::CharacterSprite()
:   x(0),
    y(0),
    lastX(0),
    lastY(0),
    z(0),
    wide(0),
    high(0),
    length(0),
    dir(""),
    srcx(0),
    srcy(0),
    srcx2(0),
    srcy2(0),
    Aframe(),
    seekx(0),
    seeky(0),
    speed(0),
    time(0),
    mph(0),
    texture(0),
    visible(false),
    parent(0),
    SpriteVerts(),
    frame(0),
    name(""),
    hp(0),
    mhp(0),
    jumpStart(0),
    jumpStrength(0),
    jumpTime(0),
    lastJump(0),
    multiJump(0),
    maxJump(0),
    flickerTime(0),
    flickOn(0),
    trueVisible(0),
    invTime(0),
    mode(""),
    kind(0),
    deathType(""),
    miscTime(0),
    color({1.0f, 1.0f, 1.0f, 1.0f}),
    soundFile(""),
    zOrder(0),
    drawTrue(false),
    mover(false),
    reverse(false),
    target(0),
    jumpM(0)
{
    // Copies this from the destroyEverything function, which looked
    // to be doing this.
    {
        auto & v = this->SpriteVerts[0];
        v.x = lp3::narrow<float>(this->x);
        v.y = lp3::narrow<float>(this->y + this->high);
        v.tu = 0;
        v.tv = 0.5;
        v.rhw = 1;
    }
    {
        auto & v = this->SpriteVerts[1];
        v.x = lp3::narrow<float>(this->x);
        v.y = lp3::narrow<float>(this->y);
        v.tu = 0;
        v.tv = 0;
        v.rhw = 1;
    }
    {
        auto & v = this->SpriteVerts[2];
        v.x = lp3::narrow<float>(this->x + this->wide);
        v.y = lp3::narrow<float>(this->y + this->high);
        v.tu = 0.5;
        v.tv = 0.5;
        v.rhw = 1;
    }
    {
        auto & v = this->SpriteVerts[3];
        v.x = lp3::narrow<float>(this->x + this->wide);
        v.y = lp3::narrow<float>(this->y);
        v.tu = 0.5;
        v.tv = 0;
        v.rhw = 1;
    }
}

}   // end namespace
