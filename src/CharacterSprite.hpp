#ifndef NND3D_CHARACTERSPRITE_HPP
#define NND3D_CHARACTERSPRITE_HPP
#pragma once

#include <array>
#include <string>
#include "AnimationFrame.hpp"
#include "Vertex.hpp"

namespace NND3d {

struct CharacterSprite
{
     double x; //Integer
     double y; //Integer
     double lastX;
     double lastY;
     int z;
     double wide; //Integer 'width and height
     double high; //Integer
     int length;
     std::string dir;

     int srcx;
     int srcy;
     int srcx2;
     int srcy2;

     std::array<AnimationFrame, 20> Aframe;
     int seekx;
     int seeky;
     double speed;  //this is tricky.  Every time this amount of seconds pass, its little routine is updated.
     double time; // this marks the time until the sprite can do its thing again.
     int mph; //refers to how many pixels the thing can haul
     int texture;

     bool visible;
     int parent; //whose their parent sprite (what is the reason for their existence?) are they a fireball cast by sprite 0, perhaps
     std::array<Vertex, 4> SpriteVerts;

     //Location As D3DVECTOR2
     int frame;
     std::string name;

     double hp; //hp!
     double mhp;

     int jumpStart;  //These are for jumping
     int jumpStrength;
     double jumpTime;
     int lastJump;
     int multiJump;
     int maxJump; //max # of jumps

     double flickerTime; //Lets them flicker until then
     bool flickOn;
     int trueVisible;
     double invTime; //how many seconds they flicker when hit

     std::string mode; //which mode are they in?
     int kind; //if kind is 0, they are neutral.  If 1, good guy.  If 2, bad guy, if 3 , good guy weapon...
     std::string deathType; //how do they die?
     double miscTime; // they must wait this long to do misc. actions
     double color; //lets throw some color into the mix

     std::string soundFile; //sound files

     int zOrder; //higher numbers are drawn last.
     bool drawTrue; //used in the draw ordering subprogram

     bool mover;

     bool reverse; //to flip bitmap or not TRUE for transposing from left to right
     int target; // it is who they are attacking

     double jumpM;


     // This initializer just zeroes everything out the way it would have been
     // in Visual Basic.
     CharacterSprite()
     :    x(0),
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
          color(0),
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
                auto v = this->SpriteVerts[0];
                v.x = this->x;
                v.y = this->y + this->high;
                v.tu = 0;
                v.tv = 0.5;
                v.rhw = 1;
            }
            {
                auto v = this->SpriteVerts[1];
                v.x = this->x;
                v.y = this->y;
                v.tu = 0;
                v.tv = 0;
                v.rhw = 1;
            }
            {
                auto v = this->SpriteVerts[2];
                v.x = this->x + this->wide;
                v.y = this->y + this->high;
                v.tu = 0.5;
                v.tv = 0.5;
                v.rhw = 1;
            }
            {
                auto v = this->SpriteVerts[3];
                v.x = this->x + this->wide;
                v.y = this->y;
                v.tu = 0.5;
                v.tv = 0;
                v.rhw = 1;
            }
     }
};


}

#endif
