struct Uniforms {
    mvp   : mat4x4f,
};

@group(0) @binding(0) var<uniform> scene : Uniforms;
@group(0) @binding(1) var tex0  : texture_2d<f32>;
@group(0) @binding(2) var samp0 : sampler;

struct VertexIn {
    @location(0) position : vec3<f32>,
    @location(1) normal   : vec3<f32>,
    @location(2) uv       : vec2<f32>,
    @location(3) tangent  : vec4<f32>,
};

struct VertexOut {
    @builtin(position) clip_pos : vec4f,
    @location(0)       uv : vec2<f32>,
};

@vertex
fn vs_main(in: VertexIn) -> VertexOut {
    var out  : VertexOut;
    out.clip_pos = scene.mvp * vec4f(in.position, 1.0);
    out.uv = in.uv;
    return out;
}

@fragment
fn fs_main(in: VertexOut) -> @location(0) vec4f {
    return textureSample(tex0, samp0, in.uv);
}
