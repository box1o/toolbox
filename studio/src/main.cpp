#include "GLFW/glfw3.h"
#include "toolbox/base/errors/result.hpp"

#include <toolbox/base/base.hpp>
#include <toolbox/gfx/gfx.hpp>

#include <toolbox/gfx/api/bindings.hpp>
#include <toolbox/gfx/api/buffer.hpp>
#include <toolbox/gfx/api/command_encoder.hpp>
#include <toolbox/gfx/api/pipeline.hpp>
#include <toolbox/gfx/api/render_pass.hpp>
#include <toolbox/gfx/api/sampler.hpp>
#include <toolbox/gfx/api/shader.hpp>
#include <toolbox/gfx/api/texture_resource.hpp>
#include <toolbox/gfx/api/vertex_layout.hpp>

#include <toolbox/math/math.hpp>

#include <array>
#include <cstddef> // offsetof

using namespace ct;

struct Vertex {
    vec3f position;
    vec3f normal;
    vec2f uv;
};

struct SceneUniform {
    mat4f mvp;
};

int main(int /*argc*/, char* /*argv*/[]) {
    log::Configure("toolbox");

    auto window = TRY(gfx::Window::Create());
    auto device = TRY(gfx::Device::Create());

    auto surface   = TRY(gfx::Surface::Create(device, window));
    auto swapchain = TRY(gfx::Swapchain::Create(device, surface));
    auto queue     = TRY(gfx::Queue::Create(device));

    swapchain->Resize(window->GetWidth(), window->GetHeight());
    window->AddResizeCallback([&](u32 w, u32 h) { swapchain->Resize(w, h); });

    // 36 vertices (per-face) so normals + UVs are correct
    const std::array<Vertex, 36> cubeVerts{{
        // +X
        {{ 1.f, -1.f, -1.f}, { 1.f, 0.f, 0.f}, {0.f, 1.f}},
        {{ 1.f,  1.f, -1.f}, { 1.f, 0.f, 0.f}, {0.f, 0.f}},
        {{ 1.f,  1.f,  1.f}, { 1.f, 0.f, 0.f}, {1.f, 0.f}},
        {{ 1.f, -1.f, -1.f}, { 1.f, 0.f, 0.f}, {0.f, 1.f}},
        {{ 1.f,  1.f,  1.f}, { 1.f, 0.f, 0.f}, {1.f, 0.f}},
        {{ 1.f, -1.f,  1.f}, { 1.f, 0.f, 0.f}, {1.f, 1.f}},
        // -X
        {{-1.f, -1.f,  1.f}, {-1.f, 0.f, 0.f}, {0.f, 1.f}},
        {{-1.f,  1.f,  1.f}, {-1.f, 0.f, 0.f}, {0.f, 0.f}},
        {{-1.f,  1.f, -1.f}, {-1.f, 0.f, 0.f}, {1.f, 0.f}},
        {{-1.f, -1.f,  1.f}, {-1.f, 0.f, 0.f}, {0.f, 1.f}},
        {{-1.f,  1.f, -1.f}, {-1.f, 0.f, 0.f}, {1.f, 0.f}},
        {{-1.f, -1.f, -1.f}, {-1.f, 0.f, 0.f}, {1.f, 1.f}},
        // +Y
        {{-1.f,  1.f, -1.f}, {0.f,  1.f, 0.f}, {0.f, 1.f}},
        {{-1.f,  1.f,  1.f}, {0.f,  1.f, 0.f}, {0.f, 0.f}},
        {{ 1.f,  1.f,  1.f}, {0.f,  1.f, 0.f}, {1.f, 0.f}},
        {{-1.f,  1.f, -1.f}, {0.f,  1.f, 0.f}, {0.f, 1.f}},
        {{ 1.f,  1.f,  1.f}, {0.f,  1.f, 0.f}, {1.f, 0.f}},
        {{ 1.f,  1.f, -1.f}, {0.f,  1.f, 0.f}, {1.f, 1.f}},
        // -Y
        {{-1.f, -1.f,  1.f}, {0.f, -1.f, 0.f}, {0.f, 1.f}},
        {{-1.f, -1.f, -1.f}, {0.f, -1.f, 0.f}, {0.f, 0.f}},
        {{ 1.f, -1.f, -1.f}, {0.f, -1.f, 0.f}, {1.f, 0.f}},
        {{-1.f, -1.f,  1.f}, {0.f, -1.f, 0.f}, {0.f, 1.f}},
        {{ 1.f, -1.f, -1.f}, {0.f, -1.f, 0.f}, {1.f, 0.f}},
        {{ 1.f, -1.f,  1.f}, {0.f, -1.f, 0.f}, {1.f, 1.f}},
        // +Z
        {{-1.f, -1.f,  1.f}, {0.f, 0.f,  1.f}, {0.f, 1.f}},
        {{ 1.f, -1.f,  1.f}, {0.f, 0.f,  1.f}, {1.f, 1.f}},
        {{ 1.f,  1.f,  1.f}, {0.f, 0.f,  1.f}, {1.f, 0.f}},
        {{-1.f, -1.f,  1.f}, {0.f, 0.f,  1.f}, {0.f, 1.f}},
        {{ 1.f,  1.f,  1.f}, {0.f, 0.f,  1.f}, {1.f, 0.f}},
        {{-1.f,  1.f,  1.f}, {0.f, 0.f,  1.f}, {0.f, 0.f}},
        // -Z
        {{ 1.f, -1.f, -1.f}, {0.f, 0.f, -1.f}, {0.f, 1.f}},
        {{-1.f, -1.f, -1.f}, {0.f, 0.f, -1.f}, {1.f, 1.f}},
        {{-1.f,  1.f, -1.f}, {0.f, 0.f, -1.f}, {1.f, 0.f}},
        {{ 1.f, -1.f, -1.f}, {0.f, 0.f, -1.f}, {0.f, 1.f}},
        {{-1.f,  1.f, -1.f}, {0.f, 0.f, -1.f}, {1.f, 0.f}},
        {{ 1.f,  1.f, -1.f}, {0.f, 0.f, -1.f}, {0.f, 0.f}},
    }};

    auto vb = TRY(gfx::Buffer::Create(device, {
        .size      = (u64)sizeof(cubeVerts),
        .usage     = gfx::BufferUsageFlags::Vertex | gfx::BufferUsageFlags::CopyDst,
        .debugName = "CubeVB",
    }));
    TRY_VOID(vb->Update(cubeVerts));

    // MVP uniform
    auto sceneUniform = TRY(gfx::Buffer::CreateUniform<SceneUniform>(device, "SceneUniform"));

    // Load texture + view + sampler
    auto tex = TRY(gfx::Texture::FromFile(device,
        "/home/toor/dev/toolbox/studio/resources/textures/beluga.png",
        {
            .usage = gfx::TextureUsageFlags::Sampled | gfx::TextureUsageFlags::CopyDst,
            .srgb = true,
            .debugName = "CatTexture",
        }));

    auto texView = TRY(tex->CreateView({.debugName = "CatTextureView"}));

    auto sampler = TRY(gfx::Sampler::Create(device, {
        .minFilter = gfx::FilterMode::Linear,
        .magFilter = gfx::FilterMode::Linear,
        .addressU  = gfx::AddressMode::Repeat,
        .addressV  = gfx::AddressMode::Repeat,
        .addressW  = gfx::AddressMode::Repeat,
        .debugName = "CatSampler",
    }));

    // Bind group layout: 0=uniform, 1=texture, 2=sampler
    auto sceneLayout = TRY(gfx::BindGroupLayout::Create(
        device,
        gfx::BindGroupLayoutDesc{}
            .Add(gfx::BindingLayoutDesc{
                .binding        = 0,
                .type           = gfx::BindingType::UniformBuffer,
                .visibility     = gfx::ShaderStageFlags::Vertex,
                .minBindingSize = sizeof(SceneUniform),
                .debugName      = "SceneUBO",
            })
            .Add(gfx::BindingLayoutDesc{
                .binding    = 1,
                .type       = gfx::BindingType::Texture2D,
                .visibility = gfx::ShaderStageFlags::Fragment,
                .debugName  = "Tex2D",
            })
            .Add(gfx::BindingLayoutDesc{
                .binding    = 2,
                .type       = gfx::BindingType::Sampler,
                .visibility = gfx::ShaderStageFlags::Fragment,
                .debugName  = "Sampler",
            })
    ));

    auto pipelineLayout = TRY(gfx::PipelineLayout::Create(
        device, gfx::PipelineLayoutDesc{}.Add(sceneLayout)
    ));

    auto sceneBindGroup = TRY(gfx::BindGroup::Create(
        device,
        gfx::BindGroupDesc{.layout = sceneLayout, .debugName = "SceneBindGroup"}
            .BindUniformBuffer(0, sceneUniform, 0, sizeof(SceneUniform))
            .BindTexture2D(1, texView)
            .BindSampler(2, sampler)
    ));

    // Shaders (use the WGSL below)
    auto vs = TRY(gfx::ShaderModule::FromFile(
        device,
        "/home/toor/dev/toolbox/studio/resources/shaders/vs_tex.wgsl",
        {.stage = gfx::ShaderStage::Vertex}));

    auto fs = TRY(gfx::ShaderModule::FromFile(
        device,
        "/home/toor/dev/toolbox/studio/resources/shaders/fs_tex.wgsl",
        {.stage = gfx::ShaderStage::Fragment}));

    // Vertex layout: pos(0), normal(1), uv(2)
    gfx::VertexLayoutDesc vld{};
    vld.Binding(0, (u32)sizeof(Vertex), gfx::VertexInputRate::PerVertex)
       .Attribute(0, 0, gfx::VertexFormat::Float3, (u32)offsetof(Vertex, position), "position")
       .Attribute(1, 0, gfx::VertexFormat::Float3, (u32)offsetof(Vertex, normal),   "normal")
       .Attribute(2, 0, gfx::VertexFormat::Float2, (u32)offsetof(Vertex, uv),       "uv");

    const bool useDepth = swapchain->HasDepth();

    auto pipeline = TRY(gfx::RenderPipeline::Create(device, {
        .vertexShader   = vs,
        .fragmentShader = fs,
        .vertexLayout   = gfx::VertexLayout{vld},
        .layout         = pipelineLayout,
        .colorFormat    = swapchain->GetColorFormat(),
        .enableDepth    = useDepth,
        .depthFormat    = useDepth ? swapchain->GetDepthFormat()
                                   : gfx::TextureFormat::Depth24PlusStencil8,
        .raster         = {
            .topology  = gfx::PrimitiveTopology::TriangleList,
            .cull      = gfx::CullMode::Back,   // safe now
            .frontFace = gfx::FrontFace::CCW,
        },
        .debugName      = "TexturedCubePipeline",
    }));

    while (!window->ShouldClose()) {
        window->PollEvents();
        device->Tick();

        auto frame   = TRY(swapchain->AcquireNextFrame());
        auto encoder = TRY(gfx::CommandEncoder::Create(device, {.debugName = "FrameEncoder"}));

        const float aspect =
            (frame.height == 0) ? 1.0f : (float)frame.width / (float)frame.height;

        const float t = (float)glfwGetTime();
        const mat4f model = rotate_y(t * 0.9f) * rotate_x(t * 0.6f);
        const mat4f view  = lookAt(vec3f(2.4f, 1.8f, 5.0f),
                                   vec3f(0.f, 0.f, 0.f),
                                   vec3f(0.f, 1.f, 0.f));
        const mat4f proj  = perspective(radians(60.0f), aspect, 0.1f, 100.0f);

        SceneUniform u{};
        u.mvp = proj * view * model;
        TRY_VOID(sceneUniform->Update(u));

        gfx::RenderPassDesc rp{};
        rp.colors = {{
            .view         = frame.colorView,
            .clear        = {0.08f, 0.09f, 0.12f, 1.0f},
            .clearEnabled = true,
        }};
        rp.enableDepth  = useDepth;
        rp.depthView    = useDepth ? frame.depthView : nullptr;
        rp.clearDepth   = 1.0f;
        rp.clearStencil = 0;
        rp.debugName    = "MainPass";

        auto pass = TRY(encoder->BeginRenderPass(rp));
        pass->BindPipeline(pipeline);
        pass->BindBindGroup(0, sceneBindGroup);
        pass->BindVertexBuffer(0, vb, 0);

        pass->Draw({.vertexCount = (u32)cubeVerts.size()});
        pass->End();

        auto cmd = TRY(encoder->Finish());
        TRY_VOID(queue->Submit({cmd}));
        TRY_VOID(swapchain->Present());
    }

    return 0;
}
