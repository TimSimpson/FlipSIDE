#ifndef NND3D_CINEMATYPE_HPP
#define NND3D_CINEMATYPE_HPP
#pragma once

#include <string>

namespace nnd3d { namespace game {

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

	CinemaType(const int frame1, const int frame2,
		const int frame3, const int frame4, const int color1,
		const int color2, const int color3, const int color4,
		const std::string & wavefile, const double miscTime);
};

}   }

#endif
