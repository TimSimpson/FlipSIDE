#ifndef NND3D_ANIMATIONFRAME_HPP
#define NND3D_ANIMATIONFRAME_HPP
#pragma once

namespace nnd3d { namespace view {

struct AnimationFrame
{
   int x;
   int y;
   int x2;
   int y2;

   AnimationFrame();

   void set(int x, int y, int x2, int y2);
};

}   }

#endif
