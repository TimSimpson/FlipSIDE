#ifndef NND3D_CAMERA_HPP
#define NND3D_CAMERA_HPP
#pragma once

#include <string>
#include <glm/glm.hpp>

namespace nnd3d { namespace game {

struct CameraGuts
{
    // TODO: Don't let random code see this state.
    int CameraX;
    int CameraY;
    int CameraWidth;
    int CameraHeight;
    int cameraStopX;  //this is where the cameraHALTS!
    int cameraStopY; //this is where the cameraHALTS! for y

    CameraGuts();
};


class Camera {
public:
    Camera(CameraGuts & guts);

    inline int x() const {
        return guts.CameraX;
    }

    inline int y() const {
        return guts.CameraY;
    }

    inline glm::vec2 ul() const {
        return glm::vec2{ x(), y() };
    }

    inline int width() const {
        return guts.CameraWidth;
    }

    inline int height() const {
        return guts.CameraHeight;
    }

    inline glm::vec2 size() const {
        return glm::vec2{ width(), height() };
    }

    inline int x2() const {
        return guts.CameraX + guts.CameraWidth;
    }

    inline int y2() const {
        return guts.CameraY + guts.CameraHeight;
    }

    // The size before the camera stops (I'm assuming there's also an implicit
    // boundary at 0,0).
    inline glm::ivec2 boundary() const {
        return glm::ivec2{guts.cameraStopX, guts.cameraStopY };
    }
private:
    CameraGuts & guts;
};

}   }

#endif
