#include "../../include/toolbox/gfx/renderer/renderer.hpp"

#include "toolbox/math/math.hpp"
#include "toolbox/base/errors/result.hpp"
#include "toolbox/gfx/api/bind_group.hpp"
#include "toolbox/gfx/api/bind_group_layout.hpp"
#include "toolbox/gfx/api/command_encoder.hpp"
#include "toolbox/gfx/api/render_pass.hpp"
#include "toolbox/gfx/api/vertex_layout.hpp"
#include <array>

namespace ct::gfx {

struct CubeVertex {
    vec3f pos;
    vec3f color;
    // f32 px, py, pz;
    // f32 r, g, b;
};

struct SceneUniform {
    ct::mat4f mvp;
};

Renderer::Renderer(ref<Window> window, const RendererDesc& desc)
    : mWindow(std::move(window)) {
    (void)desc;
}

Renderer::~Renderer() = default;

result<void> Renderer::Initialize() {
    mDevice = TRY_RETURN(Device::Create({}));


    mSwapchain = TRY_RETURN(Swapchain::Create(mDevice, mWindow));
    log::Info("Renderer: Swapchain created");

    mQueue = TRY_RETURN(Queue::Create(mDevice));
    log::Info("Renderer: Queue created");

    mShader = TRY_RETURN(
        Shader::Create(mDevice)
            .AddShaderFile(
                ShaderStage::Vertex | ShaderStage::Fragment,
                // "resources/shaders/lit.wgsl")
                "/home/pixel/dev/toolbox/studio/resources/shaders/lit.wgsl")
            .Build());

    auto sceneLayout = BindGroupLayout::Create()
        .AddUniform(0, static_cast<u8>(ShaderStage::Vertex), sizeof(SceneUniform))
        .Build();

    auto vertexLayout = VertexLayout::Create()
        .AddBuffer(VertexStepMode::Vertex)
            .Attribute(0, VertexFormat::Float32x3)
            .Attribute(1, VertexFormat::Float32x3)
        .Build();

    mPipeline = TRY_RETURN(
        Pipeline::Create(mDevice, {.label = "cube_pipeline"})
            .SetShader(mShader)
            .SetVertexLayout(vertexLayout)
            .AddBindGroupLayout(sceneLayout)
            .SetCullMode(CullMode::Back)
            .SetFrontFace(FrontFace::CCW)
            .SetTopology(PrimitiveTopology::TriangleList)
            .SetDepthTest(true)
            .SetDepthWrite(true)
            .SetDepthFormat(mSwapchain->GetDepthFormat())
            .SetColorTarget(mSwapchain->GetColorFormat())
            .Build());

    constexpr std::array<CubeVertex, 24> kCubeVertices = {{
        // +X
        {vec3f(+0.5f, -0.5f, -0.5f) , vec3f( 1.0f, 0.2f, 0.2f)}, {vec3f(+0.5f, -0.5f, +0.5f) , vec3f( 1.0f, 0.2f, 0.2f)},
        {vec3f(+0.5f, +0.5f, +0.5f) , vec3f( 1.0f, 0.2f, 0.2f)}, {vec3f(+0.5f, +0.5f, -0.5f) , vec3f( 1.0f, 0.2f, 0.2f)},
        {vec3f(-0.5f, -0.5f, +0.5f) , vec3f( 0.2f, 1.0f, 0.2f)}, {vec3f(-0.5f, -0.5f, -0.5f) , vec3f( 0.2f, 1.0f, 0.2f)},
        {vec3f(-0.5f, +0.5f, -0.5f) , vec3f( 0.2f, 1.0f, 0.2f)}, {vec3f(-0.5f, +0.5f, +0.5f) , vec3f( 0.2f, 1.0f, 0.2f)},
        {vec3f(-0.5f, +0.5f, -0.5f) , vec3f( 0.2f, 0.2f, 1.0f)}, {vec3f(+0.5f, +0.5f, -0.5f) , vec3f( 0.2f, 0.2f, 1.0f)},
        {vec3f(+0.5f, +0.5f, +0.5f) , vec3f( 0.2f, 0.2f, 1.0f)}, {vec3f(-0.5f, +0.5f, +0.5f) , vec3f( 0.2f, 0.2f, 1.0f)},
        {vec3f(-0.5f, -0.5f, +0.5f) , vec3f( 1.0f, 1.0f, 0.2f)}, {vec3f(+0.5f, -0.5f, +0.5f) , vec3f( 1.0f, 1.0f, 0.2f)},
        {vec3f(+0.5f, -0.5f, -0.5f) , vec3f( 1.0f, 1.0f, 0.2f)}, {vec3f(-0.5f, -0.5f, -0.5f) , vec3f( 1.0f, 1.0f, 0.2f)},
        {vec3f(+0.5f, -0.5f, +0.5f) , vec3f( 1.0f, 0.2f, 1.0f)}, {vec3f(-0.5f, -0.5f, +0.5f) , vec3f( 1.0f, 0.2f, 1.0f)},
        {vec3f(-0.5f, +0.5f, +0.5f) , vec3f( 1.0f, 0.2f, 1.0f)}, {vec3f(+0.5f, +0.5f, +0.5f) , vec3f( 1.0f, 0.2f, 1.0f)},
        {vec3f(-0.5f, -0.5f, -0.5f) , vec3f( 0.2f, 1.0f, 1.0f)}, {vec3f(+0.5f, -0.5f, -0.5f) , vec3f( 0.2f, 1.0f, 1.0f)},
        {vec3f(+0.5f, +0.5f, -0.5f) , vec3f( 0.2f, 1.0f, 1.0f)}, {vec3f(-0.5f, +0.5f, -0.5f) , vec3f( 0.2f, 1.0f, 1.0f)},
    }};

    constexpr std::array<u16, 36> kCubeIndices = {{
        0, 2, 1, 0, 3, 2,
        4, 6, 5, 4, 7, 6,
        8, 10, 9, 8, 11, 10,
        12, 14, 13, 12, 15, 14,
        16, 18, 17, 16, 19, 18,
        20, 22, 21, 20, 23, 22,
    }};

    mVertexBuffer = TRY_RETURN(Buffer::CreateVertexBuffer(mDevice, kCubeVertices.size() * sizeof(CubeVertex)));
    TRY_RETURN(mVertexBuffer->Update(0, kCubeVertices.data(), kCubeVertices.size() * sizeof(CubeVertex)));

    mIndexBuffer = TRY_RETURN(Buffer::CreateIndexBuffer(mDevice, kCubeIndices.size() * sizeof(u16)));
    TRY_RETURN(mIndexBuffer->Update(0, kCubeIndices.data(), kCubeIndices.size() * sizeof(u16)));
    mIndexCount = static_cast<u32>(kCubeIndices.size());

    mUniformBuffer = TRY_RETURN(Buffer::CreateUniformBuffer(mDevice, sizeof(SceneUniform)));
    SceneUniform initUniform{.mvp = ct::mat4f::identity()};
    TRY_RETURN(mUniformBuffer->Update(0, &initUniform, sizeof(initUniform)));

    mSceneBindGroup = TRY_RETURN(
        BindGroup::Create(mDevice, sceneLayout)
            .AddBuffer(0, mUniformBuffer, 0, sizeof(SceneUniform))
            .SetDebugName("scene_bind_group")
            .Build());

    log::Info("Renderer: Cube resources created");

    return ok();
}

void Renderer::BeginFrame(f32 deltaTime) {
    (void)deltaTime;

    mDevice->Tick();

    auto frameResult = mSwapchain->AcquireNextFrame();
    if (!frameResult) {
        log::Error("Renderer: failed to acquire frame");
        mFrameActive = false;
        return;
    }

    mCurrentFrame = frameResult.value();
    mFrameActive = true;
}

void Renderer::EndFrame() {
    if (!mFrameActive) return;
    mFrameActive = false;

    const f32 aspect = (mCurrentFrame.height > 0)
        ? static_cast<f32>(mCurrentFrame.width) / static_cast<f32>(mCurrentFrame.height)
        : 1.0f;

    const f32 cp = ct::cos(mOrbitPitch);
    const f32 sp = ct::sin(mOrbitPitch);
    const f32 cy = ct::cos(mOrbitYaw);
    const f32 sy = ct::sin(mOrbitYaw);
    const ct::vec3f eye{
        mOrbitDistance * cp * sy,
        mOrbitDistance * sp,
        mOrbitDistance * cp * cy,
    };

    const ct::mat4f model = ct::mat4f::identity();
    const ct::mat4f view = ct::lookAt(eye, ct::vec3f{0.0f, 0.0f, 0.0f}, ct::vec3f{0.0f, 1.0f, 0.0f});
    const ct::mat4f projection = ct::perspective(ct::radians(60.0f), aspect, 0.1f, 100.0f);

    SceneUniform scene{.mvp = projection * view * model};
    if (auto uboResult = mUniformBuffer->Update(0, &scene, sizeof(scene)); !uboResult) {
        log::Error("Renderer: failed to update scene uniform buffer");
        return;
    }

    auto encoderResult = CommandEncoder::Create(mDevice, {.debugName = "frame_encoder"});
    if (!encoderResult) {
        log::Error("Renderer: failed to create command encoder");
        return;
    }
    auto encoder = encoderResult.value();

    RenderPassDesc rpDesc{};
    rpDesc.debugName = "main_pass";

    ColorAttachmentDesc colorAttachment{};
    colorAttachment.view = mCurrentFrame.colorView;
    colorAttachment.clear = {0.08f, 0.08f, 0.10f, 1.0f};
    colorAttachment.clearEnabled = true;
    rpDesc.colors.push_back(colorAttachment);

    rpDesc.enableDepth = true;
    rpDesc.depthView = mCurrentFrame.depthView;
    rpDesc.clearDepth = 1.0f;
    rpDesc.clearStencil = 0;

    auto passResult = encoder->BeginRenderPass(rpDesc);
    if (!passResult) {
        log::Error("Renderer: failed to begin render pass");
        return;
    }
    auto pass = passResult.value();

    pass->SetPipeline(mPipeline);
    pass->SetBindGroup(0, mSceneBindGroup);
    pass->SetVertexBuffer(0, mVertexBuffer);
    pass->SetIndexBuffer(mIndexBuffer, IndexFormat::Uint16);
    pass->DrawIndexed({
        .indexCount = mIndexCount,
        .instanceCount = 1,
        .firstIndex = 0,
        .vertexOffset = 0,
        .firstInstance = 0,
    });
    pass->End();

    auto cmdResult = encoder->Finish({.debugName = "frame_commands"});
    if (!cmdResult) {
        log::Error("Renderer: failed to finish command encoder");
        return;
    }
    auto cmdBuffer = cmdResult.value();

    auto submitResult = mQueue->Submit({cmdBuffer});
    if (!submitResult) {
        log::Error("Renderer: failed to submit command buffer");
        return;
    }

    auto presentResult = mSwapchain->Present();
    if (!presentResult) {
        log::Error("Renderer: failed to present");
    }
}

void Renderer::OnEvent(events::EventBase& event) noexcept {
    events::EventDispatcher dispatcher(event);

    dispatcher.Dispatch<events::WindowResizeEvent>([&](const events::WindowResizeEvent& ev) {
        OnResize(ev.width, ev.height);
        return false;
    });

    dispatcher.Dispatch<events::MouseButtonPressedEvent>(
        [&](const events::MouseButtonPressedEvent& ev) {
            if (ev.button != events::MouseButton::Left) return false;
            mOrbitDragging = true;
            mLastMouseX = ev.x;
            mLastMouseY = ev.y;
            return false;
        });

    dispatcher.Dispatch<events::MouseButtonReleasedEvent>(
        [&](const events::MouseButtonReleasedEvent& ev) {
            if (ev.button != events::MouseButton::Left) return false;
            mOrbitDragging = false;
            return false;
        });

    dispatcher.Dispatch<events::MouseMovedEvent>([&](const events::MouseMovedEvent& ev) {
        if (!mOrbitDragging) return false;
        const f32 dx = ev.x - mLastMouseX;
        const f32 dy = ev.y - mLastMouseY;
        mLastMouseX = ev.x;
        mLastMouseY = ev.y;

        mOrbitYaw -= dx * 0.01f;
        mOrbitPitch += dy * 0.01f;
        mOrbitPitch = ct::clamp(mOrbitPitch, ct::radians(-80.0f), ct::radians(80.0f));
        return false;
    });

    dispatcher.Dispatch<events::MouseScrolledEvent>([&](const events::MouseScrolledEvent& ev) {
        mOrbitDistance -= ev.offsetY * 0.25f;
        mOrbitDistance = ct::clamp(mOrbitDistance, 1.2f, 12.0f);
        return false;
    });
}

void Renderer::OnResize(u32 width, u32 height) noexcept {
    if (!mSwapchain) return;
    mSwapchain->Resize(width, height);
}

} // namespace ct::gfx
