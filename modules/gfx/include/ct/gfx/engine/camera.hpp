#pragma once
#include <string>
#include <ct/base/types/types.hpp>

namespace ct::gfx {

struct CameraInfo{
    std::string name{"camera"};
    f32 fovY{45.0f};
    f32 aspectRatio{16.0f/9.0f};
    f32 nearPlane{0.1f};
    f32 farPlane{1000.0f};
};

class Camera{
public :
    Camera(const CameraInfo& info);
    ~Camera() = default;


private:
    CameraInfo mInfo;

};


}
