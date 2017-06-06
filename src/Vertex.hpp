#ifndef NND3D_VERTEX_HPP
#define NND3D_VERTEX_HPP
#pragma once

namespace nnd3d {

// Represents old TLVertex type, which was used with Direct3D
struct Vertex
{
    Vertex();

    float x;
    float y;
    float z;
    float rhw;
    long color;
    long specular;
    float tu;
    float tv;
};

}

#endif
