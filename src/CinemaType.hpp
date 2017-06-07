#ifndef NND3D_CINEMATYPE_HPP
#define NND3D_CINEMATYPE_HPP
#pragma once

#include <string>

namespace nnd3d {

struct CinemaType
{
    int frame1;
    int frame2;
    int frame3;
    int frame4;
    int color1;
    int color2;
    int color3;
    int color4;
    std::string wavefile;
    double miscTime;

    CinemaType();
};

}

#endif
