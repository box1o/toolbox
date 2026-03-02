#include "../../include/toolbox/gfx/renderer/renderer.hpp"

#include "toolbox/gfx/api/command_encoder.hpp"
#include "toolbox/gfx/api/render_pass.hpp"
#include "toolbox/gfx/api/vertex_layout.hpp"
#include "toolbox/base/errors/result.hpp"


namespace ct::gfx {

//NOTE: Interleaved vertex data: position(x,y) + color(r,g,b)
struct TriangleVertex {
    f32 x, y;
    f32 r, g, b;
};

// clang-format off
static constexpr TriangleVertex kTriangleVertices[] = {
    {  0.0f,  0.5f,   1.0f, 0.0f, 0.0f },
    { -0.5f, -0.5f,   0.0f, 1.0f, 0.0f },
    {  0.5f, -0.5f,   0.0f, 0.0f, 1.0f },
};
// clang-format on

static constexpr u64 kTriangleVerticesSize = sizeof(kTriangleVertices);

//NOTE: Align to 4 bytes for WebGPU WriteBuffer
static constexpr u64 AlignUp4(u64 v) noexcept { return (v + 3u) & ~3u; }

Renderer::Renderer(ref<Window> window, const RendererDesc& desc)
    : mWindow(std::move(window)) {
    (void)desc;
}

Renderer::~Renderer() = default;

result<void> Renderer::Initialize() {
    mDevice = TRY_RETURN(Device::Create({}));

    auto& info = mDevice->GetAdapterInfo();
    log::Info("Device:");
    log::Info("  Vendor: {}", info.vendor);
    log::Info("  GPU: {}", info.device);
    log::Info("  Driver: {}", info.description);
    log::Info("  maxColorAttachments: {}", mDevice->GetLimits().maxColorAttachments);

    mSwapchain = TRY_RETURN(Swapchain::Create(mDevice, mWindow));
    log::Info("Renderer: Swapchain created");

    mQueue = TRY_RETURN(Queue::Create(mDevice));
    log::Info("Renderer: Queue created");

    TRY_RETURN(CreateTriangleResources());
    log::Info("Renderer: Triangle resources created");

    return ok();
}

result<void> Renderer::CreateTriangleResources() {
    //NOTE: Shader
    mShader = TRY_RETURN(
        Shader::Create(mDevice)
            .AddShaderFile(
                ShaderStage::Vertex | ShaderStage::Fragment,
                "resources/shaders/triangle.wgsl")
            .Build());

    //NOTE: Vertex layout — position(Float32x2) + color(Float32x3)
    auto vertexLayout = VertexLayout::Create()
        .AddBuffer(VertexStepMode::Vertex)
            .Attribute(0, VertexFormat::Float32x2)
            .Attribute(1, VertexFormat::Float32x3)
        .Build();

    //NOTE: Pipeline
    mPipeline = TRY_RETURN(
        Pipeline::Create(mDevice, {.label = "triangle_pipeline"})
            .SetShader(mShader)
            .SetVertexLayout(vertexLayout)
            .SetCullMode(CullMode::None)
            .SetFrontFace(FrontFace::CCW)
            .SetTopology(PrimitiveTopology::TriangleList)
            .SetDepthTest(false)
            .SetDepthWrite(false)
            .SetColorTarget(mSwapchain->GetColorFormat())
            .Build());

    //NOTE: Vertex buffer
    u64 alignedSize = AlignUp4(kTriangleVerticesSize);

    BufferDesc vbDesc{};
    vbDesc.type = BufferType::Vertex;
    vbDesc.usage = BufferUsageFlags::Vertex | BufferUsageFlags::CopyDst;
    vbDesc.size = alignedSize;

    mVertexBuffer = TRY_RETURN(Buffer::Create(mDevice, vbDesc));

    //NOTE: Upload vertex data
    //      WriteBuffer requires size aligned to 4 — our struct is already aligned
    TRY_RETURN(mVertexBuffer->Update(0, kTriangleVertices, alignedSize));

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

    //NOTE: Create command encoder
    auto encoderResult = CommandEncoder::Create(mDevice, {.debugName = "frame_encoder"});
    if (!encoderResult) {
        log::Error("Renderer: failed to create command encoder");
        return;
    }
    auto encoder = encoderResult.value();

    //NOTE: Begin render pass
    RenderPassDesc rpDesc{};
    rpDesc.debugName = "main_pass";

    ColorAttachmentDesc colorAttachment{};
    colorAttachment.view = mCurrentFrame.colorView;
    colorAttachment.clear = {0.05f, 0.05f, 0.05f, 1.0f};
    colorAttachment.clearEnabled = true;
    rpDesc.colors.push_back(colorAttachment);

    rpDesc.enableDepth = false;

    auto passResult = encoder->BeginRenderPass(rpDesc);
    if (!passResult) {
        log::Error("Renderer: failed to begin render pass");
        return;
    }
    auto pass = passResult.value();

    //NOTE: Draw triangle
    pass->SetPipeline(mPipeline);
    pass->SetVertexBuffer(0, mVertexBuffer);
    pass->Draw({
        .vertexCount = 3,
        .instanceCount = 1,
        .firstVertex = 0,
        .firstInstance = 0,
    });
    pass->End();

    //NOTE: Finish and submit
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

    //NOTE: Present
    auto presentResult = mSwapchain->Present();
    if (!presentResult) {
        log::Error("Renderer: failed to present");
    }
}

void Renderer::OnResize(u32 width, u32 height) noexcept {
    if (!mSwapchain) return;
    mSwapchain->Resize(width, height);
}

} // namespace ct::gfx
