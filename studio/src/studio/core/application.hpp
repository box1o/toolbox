#pragma once

#include <toolbox/base/base.hpp>
#include <toolbox/gfx/gfx.hpp>

using namespace ct;

namespace studio {

class Application {
public:
    Application();
    ~Application();

    bool Update();

private:
    void InitResources();

    ref<Window> mWindow;
    ref<Device> mDevice;
    ref<Surface> mSurface;

    ref<Shader> mShader;
    ref<Buffer> mVertexBuffer;
    ref<Buffer> mIndexBuffer;
    ref<Buffer> mUniformBuffer;
    ref<BindGroupLayout> mBindGroupLayout;
    ref<BindGroup> mBindGroup;
    ref<RenderPipeline> mPipeline;

    float mAngle = 0.0f;
};

} // namespace studio
