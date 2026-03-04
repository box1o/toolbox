#include "GLFW/glfw3.h"
#include "toolbox/gfx/api/bind_group_layout.hpp"
#include "toolbox/gfx/api/command_encoder.hpp"
#include "toolbox/gfx/api/device.hpp"
#include "toolbox/gfx/api/queue.hpp"
#include "toolbox/gfx/api/render_pass.hpp"
#include "toolbox/gfx/api/sampler.hpp"
#include "toolbox/gfx/api/shader.hpp"
#include "toolbox/gfx/api/swapchain.hpp"
#include "toolbox/gfx/api/texture.hpp"
#include "toolbox/gfx/api/vertex_layout.hpp"
#include "toolbox/gfx/camera/editor_camera.hpp"
#include "toolbox/gfx/events/dispatcher.hpp"
#include "toolbox/gfx/events/window/events.hpp"
#include "toolbox/gfx/window/window.hpp"
#include "toolbox/math/interop/transform.hpp"
#include <toolbox/base/base.hpp>
#include <toolbox/gfx/gfx.hpp>
#include <toolbox/math/math.hpp>

#include <array>

using namespace ct;

struct StaticVertex {
    vec3f position;
    vec3f normal;
    vec2f uv;
    vec4f tangent;

    static constexpr u32 Stride() { return sizeof(StaticVertex); }
};

struct Uniforms {
    mat4f mvp;
};

std::array<StaticVertex, 24> kVertices = {{
    // Back (-Z)
    {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}, {1, 0, 0, 1}},
    {{0.5f, -0.5f, -0.5f},  {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}, {1, 0, 0, 1}},
    {{0.5f, 0.5f, -0.5f},   {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f}, {1, 0, 0, 1}},
    {{-0.5f, 0.5f, -0.5f},  {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}, {1, 0, 0, 1}},

    // Front (+Z)
    {{-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}, {1, 0, 0, 1}},
    {{0.5f, -0.5f, 0.5f},  {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}, {1, 0, 0, 1}},
    {{0.5f, 0.5f, 0.5f},   {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}, {1, 0, 0, 1}},
    {{-0.5f, 0.5f, 0.5f},  {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}, {1, 0, 0, 1}},

    // Bottom (-Y)
    {{-0.5f, -0.5f, 0.5f},  {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f}, {1, 0, 0, 1}},
    {{0.5f, -0.5f, 0.5f},   {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f}, {1, 0, 0, 1}},
    {{0.5f, -0.5f, -0.5f},  {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f}, {1, 0, 0, 1}},
    {{-0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f}, {1, 0, 0, 1}},

    // Top (+Y)
    {{-0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}, {1, 0, 0, 1}},
    {{0.5f, 0.5f, -0.5f},  {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}, {1, 0, 0, 1}},
    {{0.5f, 0.5f, 0.5f},   {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}, {1, 0, 0, 1}},
    {{-0.5f, 0.5f, 0.5f},  {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}, {1, 0, 0, 1}},

    // Right (+X)
    {{0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}, {0, 0, -1, 1}},
    {{0.5f, -0.5f, 0.5f},  {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}, {0, 0, -1, 1}},
    {{0.5f, 0.5f, 0.5f},   {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}, {0, 0, -1, 1}},
    {{0.5f, 0.5f, -0.5f},  {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}, {0, 0, -1, 1}},

    // Left (-X)
    {{-0.5f, -0.5f, 0.5f},  {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}, {0, 0, 1, 1}},
    {{-0.5f, -0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}, {0, 0, 1, 1}},
    {{-0.5f, 0.5f, -0.5f},  {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}, {0, 0, 1, 1}},
    {{-0.5f, 0.5f, 0.5f},   {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}, {0, 0, 1, 1}},
}};

std::array<u16, 36> kIndices = {{
    0, 1, 2, 0, 2, 3,       // back
    4, 5, 6, 4, 6, 7,       // front
    8, 9, 10, 8, 10, 11,    // bottom
    12, 13, 14, 12, 14, 15, // top
    16, 17, 18, 16, 18, 19, // right
    20, 21, 22, 20, 22, 23  // left
}};

int main(int argc, char* argv[]) {
    log::Configure("toolbox");

    auto window = TRY(gfx::Window::Create());
    auto device = TRY(gfx::Device::Create());
    auto swapchain = TRY(gfx::Swapchain::Create(device, window, {}));

    EditorCamera camera;

    window->SetEventCallback([&](ct::events::EventBase& event) {
        events::EventDispatcher dispatcher(event);
        camera.OnEvent(event);

        dispatcher.Dispatch<events::WindowResizeEvent>([&](const events::WindowResizeEvent& ev) {
            camera.SetViewportSize(ev.width, ev.height);
            swapchain->Resize(ev.width, ev.height);
            return false;
        });
    });

    auto queue = TRY(gfx::Queue::Create(device));



    // render elements
    auto shader = TRY(gfx::Shader::Create(device)
            .AddShaderFile(gfx::ShaderStage::Vertex | gfx::ShaderStage::Fragment,
                "/home/pixel/dev/toolbox/studio/resources/shaders/unlit.wgsl")
            .Build());

    auto layout = gfx::VertexLayout::Create()
                      .AddBuffer(gfx::VertexStepMode::Vertex)
                      .Attribute(0, gfx::VertexFormat::Float32x3)
                      .Attribute(1, gfx::VertexFormat::Float32x3)
                      .Attribute(2, gfx::VertexFormat::Float32x2)
                      .Attribute(3, gfx::VertexFormat::Float32x4)
                      .Build();

    auto bLayout = gfx::BindGroupLayout::Create()
                       .AddUniform(0, static_cast<u8>(gfx::ShaderStage::Vertex), sizeof(Uniforms))
                       .AddTexture(1, static_cast<u8>(gfx::ShaderStage::Fragment))
                       .AddSampler(2, static_cast<u8>(gfx::ShaderStage::Fragment))
                       .Build();

    auto pipeline = TRY(gfx::Pipeline::Create(device)
            .SetShader(shader)
            .SetVertexLayout(layout)
            .AddBindGroupLayout(bLayout)
            .SetCullMode(gfx::CullMode::None)
            .SetFrontFace(gfx::FrontFace::CW)
            .SetTopology(gfx::PrimitiveTopology::TriangleList)
            .SetDepthTest(true)
            .SetDepthWrite(true)
            .SetDepthFormat(swapchain->GetDepthFormat())
            .SetColorTarget(swapchain->GetColorFormat())
            .Build());

    auto vbo =
        TRY(gfx::Buffer::CreateVertexBuffer(device, kVertices.size() * sizeof(StaticVertex)));
    TRY(vbo->Update(0, kVertices.data(), kVertices.size() * sizeof(StaticVertex)));

    auto ibo = TRY(gfx::Buffer::CreateIndexBuffer(device, kIndices.size() * sizeof(u16)));
    TRY(ibo->Update(0, kIndices.data(), kIndices.size() * sizeof(u16)));
    // auto indexCount= static_cast<u32>(kCubeIndices.size());

    auto ubo = TRY(gfx::Buffer::CreateUniformBuffer(device, sizeof(Uniforms)));

    Uniforms uData{.mvp = ct::mat4f::identity()};
    TRY(ubo->Update(0, &uData, sizeof(uData)));


    auto texture = TRY(gfx::Texture::FromFile(device, "resources/textures/beluga.png"));
    auto textureView = TRY(texture->CreateView());
    auto sampler = TRY(gfx::Sampler::Create(device));

    auto bindGroup =
        TRY(gfx::BindGroup::Create(device, bLayout)
                . AddBuffer(0, ubo, 0, sizeof(Uniforms))
                .AddTexture(1, textureView)
                .AddSampler(2, sampler)
                .Build());


    // NOTE: Second render target

    while (!window->ShouldClose()) {
        auto frame = TRY(swapchain->AcquireNextFrame());

        auto encoder = TRY(gfx::CommandEncoder::Create(device));
        auto time = glfwGetTime();
        auto model = rotate<float>((float)time, {1.0f, 1.0f, 1.0f});
        // auto view = lookAt<float>({0.0f , 0.0f , -5.0f}, {0.0f , 0.0f , 0.0f} , {0.0f , 1.0f ,
        // 0.0f}); auto proj = perspective<float>(45.0f, (float)window->GetAspectRatio(), 0.001f,
        // 10000.0f);

        auto cam = camera.GetCamera();
        auto mv = cam.GetProjectionMatrix() * cam.GetViewMatrix();

        Uniforms uData{.mvp = mv * model};
        TRY(ubo->Update(0, &uData, sizeof(uData)));

        // NOTE: Attachments
        gfx::ColorAttachmentDesc caDesc;
        caDesc.clear = {0.1f, 0.1f, 0.1f, 1.0f};
        caDesc.view = frame.colorView;

        // NOTE: render pass
        gfx::RenderPassDesc rpDesc;
        rpDesc.enableDepth = true;
        rpDesc.depthView = frame.depthView;
        rpDesc.colors.push_back(caDesc);
        auto rpass = TRY(encoder->BeginRenderPass(rpDesc));
        rpass->SetBindGroup(0, bindGroup);
        rpass->SetPipeline(pipeline);
        rpass->SetVertexBuffer(0, vbo);
        rpass->SetIndexBuffer(ibo, gfx::IndexFormat::Uint16);

        rpass->DrawIndexed({.indexCount = kIndices.size()});

        rpass->End();

        auto cmd = TRY(encoder->Finish());
        TRY(queue->Submit({cmd}));
        TRY(swapchain->Present());
        window->PollEvents();
    }
}
