@group(0) @binding(0) var previewTex : texture_2d<f32>;
@group(0) @binding(1) var previewSampler : sampler;

struct VertexOut {
    @builtin(position) clip_pos : vec4f,
    @location(0) uv : vec2f,
};

@vertex
fn vs_main(@builtin(vertex_index) vi : u32) -> VertexOut {
    var out : VertexOut;

    // Full-screen triangle. Viewport/scissor will place it in top-right.
    let p = array<vec2f, 3>(
        vec2f(-1.0, -1.0),
        vec2f( 3.0, -1.0),
        vec2f(-1.0,  3.0)
    );

    let clip = p[vi];
    out.clip_pos = vec4f(clip, 0.0, 1.0);
    let uv = clip * 0.5 + vec2f(0.5, 0.5);
    out.uv = vec2f(uv.x, 1.0 - uv.y);
    return out;
}

@fragment
fn fs_main(in: VertexOut) -> @location(0) vec4f {
    return textureSample(previewTex, previewSampler, in.uv);
}
