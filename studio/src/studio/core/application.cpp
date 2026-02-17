#include "studio/core/application.hpp"
#include <toolbox/math/math.hpp>

static constexpr const char* kCubeShader = R"(
struct Uniforms {
    mvp : mat4x4<f32>,
    model : mat4x4<f32>,
};
@group(0) @binding(0) var<uniform> u : Uniforms;

struct VsIn {
    @location(0) position : vec3<f32>,
    @location(1) normal : vec3<f32>,
    @location(2) color : vec3<f32>,
};

struct VsOut {
    @builtin(position) position : vec4<f32>,
    @location(0) worldNormal : vec3<f32>,
    @location(1) color : vec3<f32>,
};

@vertex
fn vs_main(in : VsIn) -> VsOut {
    var out : VsOut;
    out.position = u.mvp * vec4<f32>(in.position, 1.0);
    out.worldNormal = (u.model * vec4<f32>(in.normal, 0.0)).xyz;
    out.color = in.color;
    return out;
}

@fragment
fn fs_main(in : VsOut) -> @location(0) vec4<f32> {
    let lightDir = normalize(vec3<f32>(0.5, 1.0, 0.8));
    let n = normalize(in.worldNormal);
    let ambient = 0.15;
    let diffuse = max(dot(n, lightDir), 0.0);
    let lit = ambient + diffuse * 0.85;
    return vec4<f32>(in.color * lit, 1.0);
}
)";

struct Vertex {
    float position[3];
    float normal[3];
    float color[3];
};

static constexpr Vertex kCubeVertices[] = {
    {{-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.95f, 0.42f, 0.20f}},
    {{0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.95f, 0.42f, 0.20f}},
    {{0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.95f, 0.42f, 0.20f}},
    {{-0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.95f, 0.42f, 0.20f}},

    {{0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.22f, 0.67f, 0.98f}},
    {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.22f, 0.67f, 0.98f}},
    {{-0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.22f, 0.67f, 0.98f}},
    {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.22f, 0.67f, 0.98f}},

    {{0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}, {0.96f, 0.86f, 0.28f}},
    {{0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.96f, 0.86f, 0.28f}},
    {{0.5f, 0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.96f, 0.86f, 0.28f}},
    {{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}, {0.96f, 0.86f, 0.28f}},

    {{-0.5f, -0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {0.38f, 0.92f, 0.45f}},
    {{-0.5f, -0.5f, 0.5f}, {-1.0f, 0.0f, 0.0f}, {0.38f, 0.92f, 0.45f}},
    {{-0.5f, 0.5f, 0.5f}, {-1.0f, 0.0f, 0.0f}, {0.38f, 0.92f, 0.45f}},
    {{-0.5f, 0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {0.38f, 0.92f, 0.45f}},

    {{-0.5f, 0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}, {0.85f, 0.32f, 0.78f}},
    {{0.5f, 0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}, {0.85f, 0.32f, 0.78f}},
    {{0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.85f, 0.32f, 0.78f}},
    {{-0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.85f, 0.32f, 0.78f}},

    {{-0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {0.20f, 0.80f, 0.60f}},
    {{0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {0.20f, 0.80f, 0.60f}},
    {{0.5f, -0.5f, 0.5f}, {0.0f, -1.0f, 0.0f}, {0.20f, 0.80f, 0.60f}},
    {{-0.5f, -0.5f, 0.5f}, {0.0f, -1.0f, 0.0f}, {0.20f, 0.80f, 0.60f}},
};

static constexpr uint16_t kCubeIndices[] = {
    0,
    1,
    2,
    0,
    2,
    3,
    4,
    5,
    6,
    4,
    6,
    7,
    8,
    9,
    10,
    8,
    10,
    11,
    12,
    13,
    14,
    12,
    14,
    15,
    16,
    17,
    18,
    16,
    18,
    19,
    20,
    21,
    22,
    20,
    22,
    23,
};

// NOTE: 2 × mat4 = 128 bytes
static constexpr uint32_t kUniformSize = 128;

namespace studio {

Application::Application() {
    mWindow = TRY(Window::Create());
    mDevice = TRY(Device::Create());
    mSurface = TRY(Surface::Create(mWindow, mDevice, {}));
    InitResources();
}

void Application::InitResources() {
    mShader = TRY(Shader::Create(mDevice, ShaderInfo::FromSource(kCubeShader, "cube")));

    mVertexBuffer = TRY(Buffer::Create(mDevice, BufferType::Vertex, std::span{kCubeVertices}));
    mIndexBuffer = TRY(Buffer::Create(mDevice, BufferType::Index, std::span{kCubeIndices}));
    mUniformBuffer = TRY(Buffer::Create(mDevice, BufferInfo::Uniform(kUniformSize)));

    mBindGroupLayout = TRY(BindGroupLayout::Create(
        mDevice, BindGroupLayoutInfo{}.AddUniform(0, kUniformSize, wgpu::ShaderStage::Vertex)));

    mBindGroup = TRY(BindGroup::Create(
        mDevice, BindGroupInfo{}.SetLayout(mBindGroupLayout).AddBuffer(0, mUniformBuffer)));

    auto layout = VertexLayout{}
                      .AddBuffer(sizeof(Vertex))
                      .AddAttribute(0, VertexFormat::Float3, offsetof(Vertex, position))
                      .AddAttribute(1, VertexFormat::Float3, offsetof(Vertex, normal))
                      .AddAttribute(2, VertexFormat::Float3, offsetof(Vertex, color));

    mPipeline =
        TRY(RenderPipeline::Create(mDevice, RenderPipelineInfo{}
                                                .SetShader(mShader)
                                                .SetVertexLayout(layout)
                                                .AddBindGroupLayout(mBindGroupLayout)
                                                .AddColorFormat(mSurface->GetFormat())
                                                .SetDepthStencil(DepthStencilState::Default())));
}

Application::~Application() {}

bool Application::Update() {
    mWindow->PollEvents();
    if (mWindow->ShouldClose()) return true;

    mAngle += 0.01f;

    ct::mat4f model = ct::rotate_y(mAngle) * ct::rotate_x(mAngle * 0.3f);
    ct::mat4f view = ct::lookAt(
        ct::vec3f(0.0f, 1.5f, 3.0f), ct::vec3f(0.0f, 0.0f, 0.0f), ct::vec3f(0.0f, 1.0f, 0.0f));
    ct::mat4f proj =
        ct::perspective(ct::pi<float> / 4.0f, mSurface->GetAspectRatio(), 0.1f, 100.0f);

    ct::mat4f mvp = proj * view * model;

    struct {
        ct::mat4f mvp;
        ct::mat4f model;
    } uniforms{mvp, model};

    mUniformBuffer->Write(uniforms);

    auto frameResult = mSurface->BeginFrame();
    if (!frameResult) return false;
    auto frame = frameResult.value();

    auto passResult = RenderPass::Begin(*mDevice, frame, {0.08, 0.08, 0.10, 1.0});
    if (!passResult) return false;
    auto pass = std::move(passResult.value());

    pass.SetPipeline(*mPipeline);
    pass.SetBindGroup(0, *mBindGroup);
    pass.SetVertexBuffer(0, *mVertexBuffer);
    pass.SetIndexBuffer(*mIndexBuffer, IndexFormat::U16);
    pass.DrawIndexed(36);
    pass.Submit();

    mSurface->Present();
    return false;
}

} // namespace studio
