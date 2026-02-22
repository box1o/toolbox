#include <toolbox/base/base.hpp>
#include <toolbox/gfx/gfx.hpp>
#include <toolbox/math/math.hpp>

#include <chrono>

using namespace ct;

struct Vertex {
    float px, py, pz;
    float nx, ny, nz;
};

struct SceneUniforms {
    mat4f mvp;
    mat4f model;
    vec4f lightDir;
    vec4f lightColor;
    vec4f baseColor;
};

int main() {
    log::Configure("toolbox");

    auto window = TRY(gfx::Window::Create());

    auto device = TRY(gfx::Device::Create());

    auto surface = TRY(gfx::Surface::Create(device, window, {.debugName = "Surface"}));

    auto swapchain = TRY(gfx::Swapchain::Create(device, surface, {}));

    auto queue = TRY(gfx::Queue::Create(device, {.type = gfx::QueueType::Graphics}));

    swapchain->Resize(window->GetWidth(), window->GetHeight());

    window->AddResizeCallback([&](u32 w, u32 h) { swapchain->Resize(w, h); });

    const Vertex cubeVerts[] = {
        // +X
        {1.f, -1.f, -1.f, 1.f, 0.f, 0.f},
        {1.f, 1.f, -1.f, 1.f, 0.f, 0.f},
        {1.f, 1.f, 1.f, 1.f, 0.f, 0.f},
        {1.f, -1.f, -1.f, 1.f, 0.f, 0.f},
        {1.f, 1.f, 1.f, 1.f, 0.f, 0.f},
        {1.f, -1.f, 1.f, 1.f, 0.f, 0.f},
        // -X
        {-1.f, -1.f, 1.f, -1.f, 0.f, 0.f},
        {-1.f, 1.f, 1.f, -1.f, 0.f, 0.f},
        {-1.f, 1.f, -1.f, -1.f, 0.f, 0.f},
        {-1.f, -1.f, 1.f, -1.f, 0.f, 0.f},
        {-1.f, 1.f, -1.f, -1.f, 0.f, 0.f},
        {-1.f, -1.f, -1.f, -1.f, 0.f, 0.f},
        // +Y
        {-1.f, 1.f, -1.f, 0.f, 1.f, 0.f},
        {-1.f, 1.f, 1.f, 0.f, 1.f, 0.f},
        {1.f, 1.f, 1.f, 0.f, 1.f, 0.f},
        {-1.f, 1.f, -1.f, 0.f, 1.f, 0.f},
        {1.f, 1.f, 1.f, 0.f, 1.f, 0.f},
        {1.f, 1.f, -1.f, 0.f, 1.f, 0.f},
        // -Y
        {-1.f, -1.f, 1.f, 0.f, -1.f, 0.f},
        {-1.f, -1.f, -1.f, 0.f, -1.f, 0.f},
        {1.f, -1.f, -1.f, 0.f, -1.f, 0.f},
        {-1.f, -1.f, 1.f, 0.f, -1.f, 0.f},
        {1.f, -1.f, -1.f, 0.f, -1.f, 0.f},
        {1.f, -1.f, 1.f, 0.f, -1.f, 0.f},
        // +Z
        {-1.f, -1.f, 1.f, 0.f, 0.f, 1.f},
        {1.f, -1.f, 1.f, 0.f, 0.f, 1.f},
        {1.f, 1.f, 1.f, 0.f, 0.f, 1.f},
        {-1.f, -1.f, 1.f, 0.f, 0.f, 1.f},
        {1.f, 1.f, 1.f, 0.f, 0.f, 1.f},
        {-1.f, 1.f, 1.f, 0.f, 0.f, 1.f},
        // -Z
        {1.f, -1.f, -1.f, 0.f, 0.f, -1.f},
        {-1.f, -1.f, -1.f, 0.f, 0.f, -1.f},
        {-1.f, 1.f, -1.f, 0.f, 0.f, -1.f},
        {1.f, -1.f, -1.f, 0.f, 0.f, -1.f},
        {-1.f, 1.f, -1.f, 0.f, 0.f, -1.f},
        {1.f, 1.f, -1.f, 0.f, 0.f, -1.f},
    };

    auto vb = TRY(gfx::Buffer::Create(
        device, {
                    .size = sizeof(cubeVerts),
                    .usage = gfx::BufferUsageFlags::Vertex | gfx::BufferUsageFlags::CopyDst,
                    .debugName = "CubeVB",
                }));
    if (auto upload = vb->Update(cubeVerts); !upload) {
        log::Critical("Cube vertex upload failed: {}", upload.error().Message());
        return 1;
    }

    auto sceneUniform = TRY(gfx::Buffer::Create(
        device, {
                    .size = sizeof(SceneUniforms),
                    .usage = gfx::BufferUsageFlags::Uniform | gfx::BufferUsageFlags::CopyDst,
                    .debugName = "SceneUniform",
                }));

    auto vs = TRY(gfx::ShaderModule::FromFile(device,
        "/home/toor/dev/toolbox/studio/resources/shaders/cube.vert.wgsl",
        {
            .stage = gfx::ShaderStage::Vertex,
            .entryPoint = "main",
            .debugName = "CubeVS",
        }));
    auto fs = TRY(gfx::ShaderModule::FromFile(device,
        "/home/toor/dev/toolbox/studio/resources/shaders/cube.frag.wgsl",
        {
            .stage = gfx::ShaderStage::Fragment,
            .entryPoint = "main",
            .debugName = "CubeFS",
        }));

    auto sceneLayout = TRY(gfx::BindGroupLayout::Create(
        device, gfx::BindGroupLayoutDesc{}.Add(gfx::BindingLayoutDesc{
                    .binding = 0,
                    .type = gfx::BindingType::UniformBuffer,
                    .visibility = gfx::ShaderStageFlags::Vertex | gfx::ShaderStageFlags::Fragment,
                    .minBindingSize = sizeof(SceneUniforms),
                    .debugName = "SceneUniformBinding",
                })));

    auto pipelineLayout =
        TRY(gfx::PipelineLayout::Create(device, gfx::PipelineLayoutDesc{}.Add(sceneLayout)));

    gfx::BindGroupDesc sceneBgDesc{};
    sceneBgDesc.layout = sceneLayout;
    sceneBgDesc.debugName = "SceneBindGroup";
    sceneBgDesc.BindUniformBuffer(0, sceneUniform, 0, sizeof(SceneUniforms));
    auto sceneBindGroup = TRY(gfx::BindGroup::Create(device, sceneBgDesc));

    gfx::VertexLayout layout(gfx::VertexLayoutDesc{}
            .Binding(0, sizeof(Vertex), gfx::VertexInputRate::PerVertex)
            .Attribute(0, 0, gfx::VertexFormat::Float3, 0, "position")
            .Attribute(1, 0, gfx::VertexFormat::Float3, 12, "normal"));

    auto pipeline = TRY(gfx::RenderPipeline::Create(
        device, {
                    .vertexShader = vs,
                    .fragmentShader = fs,
                    .vertexLayout = layout,
                    .layout = pipelineLayout,
                    .colorFormat = swapchain->GetColorFormat(),
                    .enableDepth = true,
                    .depthFormat = gfx::TextureFormat::Depth24PlusStencil8,
                    .raster =
                        gfx::RasterState{
                            .topology = gfx::PrimitiveTopology::TriangleList,
                            .cull = gfx::CullMode::Back,
                            .frontFace = gfx::FrontFace::CCW,
                        },
                    .debugName = "CubePipeline",
                }));
    const auto startTime = std::chrono::steady_clock::now();

    while (!window->ShouldClose()) {
        window->PollEvents();
        device->Tick();

        auto frame = TRY(swapchain->AcquireNextFrame());
        auto enc = TRY(gfx::CommandEncoder::Create(device, {.debugName = "FrameEncoder"}));

        const auto now = std::chrono::steady_clock::now();
        const float t = std::chrono::duration<float>(now - startTime).count();
        const float aspect = static_cast<float>(frame.width) /
                             static_cast<float>(frame.height == 0 ? 1 : frame.height);

        const mat4f model = rotate_y(t * 0.9f) * rotate_x(t * 0.6f);
        const mat4f view =
            lookAt(vec3f(2.4f, 1.8f, 5.0f), vec3f(0.f, 0.f, 0.f), vec3f(0.f, 1.f, 0.f));
        const mat4f proj = perspective(radians(60.0f), aspect, 0.1f, 100.0f);

        SceneUniforms scene{};
        scene.mvp = proj * view * model;
        scene.model = model;
        scene.lightDir = vec4f(-0.45f, -1.0f, -0.35f, 0.0f);
        scene.lightColor = vec4f(1.0f, 0.98f, 0.95f, 1.0f);
        scene.baseColor = vec4f(0.95f, 0.48f, 0.18f, 1.0f);

        if (auto upload = sceneUniform->Update(scene); !upload) {
            log::Critical("Scene uniform upload failed: {}", upload.error().Message());
            break;
        }

        gfx::RenderPassDesc rp{};
        rp.debugName = "CubePass";
        rp.enableDepth = true;
        rp.depthView = frame.depthView;
        rp.clearDepth = 1.0f;

        gfx::ColorAttachmentDesc c0{};
        c0.view = frame.colorView;
        c0.clear = {0.08f, 0.09f, 0.12f, 1.0f};
        c0.clearEnabled = true;
        rp.colors.push_back(c0);

        auto pass = TRY(enc->BeginRenderPass(rp));
        pass->BindPipeline(pipeline);
        pass->BindBindGroup(0, sceneBindGroup);
        pass->BindVertexBuffer(0, vb, 0);
        pass->Draw({.vertexCount = static_cast<u32>(sizeof(cubeVerts) / sizeof(cubeVerts[0])),
            .instanceCount = 1,
            .firstVertex = 0,
            .firstInstance = 0});
        pass->End();

        auto cmd = TRY(enc->Finish());
        if (auto submit = queue->Submit({cmd}); !submit) {
            log::Critical("Queue submit failed: {}", submit.error().Message());
            break;
        }
        if (auto present = swapchain->Present(); !present) {
            log::Critical("Present failed: {}", present.error().Message());
            break;
        }
    }
    return 0;
}
