#ifndef NND3D_VERTEX_HPP
#define NND3D_VERTEX_HPP
#pragma once

namespace NND3d {

// Represents old TLVertex type, which was used with Direct3D
struct Vertex
{
    float x;
    float y;
    float z;
    float rhw;
    long color;
    long specular;
    float tu;
    float tv;

    public Vertex()
    :   x(0),
        y(0),
        z(0),
        rhw(0),
        color(0),
        specular(0),
        tu(0),
        tv(0)
    {
    }
};

}

#endif
