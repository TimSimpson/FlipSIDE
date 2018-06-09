#ifndef NND3D_CAMERA_HPP
#define NND3D_CAMERA_HPP
#pragma once

#include <string>
#include <glm/glm.hpp>

namespace nnd3d { namespace game {

struct CameraGuts
{
    CameraGuts();

private:
    int CameraX;
    int CameraY;

    int CameraWidth;
    int CameraHeight;

    int cameraStopX;  //this is where the cameraHALTS!
    int cameraStopY; //this is where the cameraHALTS! for y

    friend class Camera;
};


// ---------------------------------------------------------------------------
// class Camera
// ---------------------------------------------------------------------------
//     Represents the box currently being looked at by the game.
// ---------------------------------------------------------------------------
class Camera {
public:
    Camera(CameraGuts & guts);

    // Left most coordinate seen by the camera.
    inline int x() const {
        return guts.CameraX;
    }

    // upper most coordinate seen by camera.
    inline int y() const {
        return guts.CameraY;
    }

    // upper-left corner
    inline glm::ivec2 ul() const {
        return glm::vec2{ x(), y() };
    }

    inline int width() const {
        return guts.CameraWidth;
    }

    inline int height() const {
        return guts.CameraHeight;
    }

    inline glm::ivec2 size() const {
        return glm::vec2{ width(), height() };
    }

    // right most coordinate seen by camera
    inline int x2() const {
        return guts.CameraX + guts.CameraWidth;
    }

    // bottom most coordinate seen by camera
    inline int y2() const {
        return guts.CameraY + guts.CameraHeight;
    }

    // TODO: split boundary into it's own class.

    // The size before the camera stops (I'm assuming there's also an implicit
    // boundary at 0,0).
    inline glm::ivec2 boundary() const {
        return glm::ivec2{guts.cameraStopX, guts.cameraStopY };
    }

    void focus(const glm::ivec2 & center, bool enforce_boundary=true);

    void set_boundary(const glm::ivec2 & new_boundary);

    void set_size(const glm::ivec2 & new_size);
private:
    CameraGuts & guts;
};

}   }

#endif
