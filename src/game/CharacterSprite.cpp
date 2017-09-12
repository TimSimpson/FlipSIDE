#include "CharacterSprite.hpp"

namespace nnd3d { namespace game {

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
    jumpM(0),
    proc()
{
}

// Makes the sprite's size match it's current Animation Frame
void unstretch(CharacterSprite & s) {
	if (s.frame == 0) {
		s.wide = s.srcx2 - s.srcx;
		s.high = s.srcy2 = s.srcy;
	}
	else {
		s.wide = s.Aframe[s.frame].x2 - s.Aframe[s.frame].x;
		s.high = s.Aframe[s.frame].y2 - s.Aframe[s.frame].y;
	}
}

}   }   // end namespace
