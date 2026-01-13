#pragma once

#include "ct/gfx/engine/camera.hpp"
#include <ct/base/base.hpp>
#include "ct/gfx/gfx.hpp"


namespace ct::gfx {


// enum class PassType{
//     Opaque,
//     Transparent,
//     PostProcess,
//     Shadow,
//     UI
// };



struct RendererInfo {

};


class Renderer{
public:
    Renderer(const RendererInfo& info);
    ~Renderer() = default;

    void Begin(const ref<Camera>& camera);
    void End();

private:
    RendererInfo mInfo;
    scope<gfx::Window> mWindow;
    ref<gfx::Device> mDevice;

};
}


