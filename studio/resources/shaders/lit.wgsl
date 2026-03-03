// struct SceneUniform {
//     mvp   : mat4x4f,
//     time  : f32,
// };
//
// @group(0) @binding(0) var<uniform> scene : SceneUniform;
// @group(0) @binding(1) var tex0  : texture_2d<f32>;
// @group(0) @binding(2) var samp0 : sampler;
//
// struct VertexIn {
//     @location(0) pos : vec3f,
//     @location(1) uv  : vec2f,
//     @location(2) nrm : vec3f,
// };
//
// struct VertexOut {
//     @builtin(position) clip_pos : vec4f,
//     @location(0)       uv       : vec2f,
//     @location(1)       nrm      : vec3f,
// };
//
// @vertex
// fn vs_main(in: VertexIn) -> VertexOut {
//     var out  : VertexOut;
//     out.clip_pos = scene.mvp * vec4f(in.pos, 1.0);
//     out.uv       = in.uv;
//     out.nrm      = in.nrm;
//     return out;
// }
//
// @fragment
// fn fs_main(in: VertexOut) -> @location(0) vec4f {
//     let base = textureSample(tex0, samp0, in.uv);
//     return vec4f(base);
// }
//
//



struct SceneUniform {
    mvp   : mat4x4f,
};

@group(0) @binding(0) var<uniform> scene : SceneUniform;

struct VertexIn {
    @location(0) pos : vec3f,
    @location(1) col : vec3f,
};

struct VertexOut {
    @builtin(position) clip_pos : vec4f,
    @location(0)       col : vec3f,
};

@vertex
fn vs_main(in: VertexIn) -> VertexOut {
    var out  : VertexOut;
    out.clip_pos = scene.mvp * vec4f(in.pos, 1.0);
    out.col = in.col;
    return out;
}

@fragment
fn fs_main(in: VertexOut) -> @location(0) vec4f {
    let base = vec4f(in.col , 1.0f);
    return vec4f(base);
}
