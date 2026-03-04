struct SceneUniform {
    mvp : mat4x4f,
};

@group(0) @binding(0) var<uniform> scene : SceneUniform;

struct VertexIn {
    @location(0) pos : vec3f,
    @location(1) color : vec4f,
};

struct VertexOut {
    @builtin(position) clip_pos : vec4f,
    @location(0) color : vec4f,
};

struct FragmentOut {
    @location(0) color : vec4f,
    @location(1) normal : vec4f,
};

@vertex
fn vs_main(in: VertexIn) -> VertexOut {
    var out : VertexOut;
    out.clip_pos = scene.mvp * vec4f(in.pos, 1.0);
    out.color = in.color;
    return out;
}

@fragment
fn fs_main(in: VertexOut) -> FragmentOut {
    var out : FragmentOut;
    out.color = in.color;
    out.normal = vec4f(0.5, 0.5, 1.0, 1.0);
    return out;
}
