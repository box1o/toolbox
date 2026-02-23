struct SceneUniform {
  mvp: mat4x4f,
  time: f32,
};

@group(0) @binding(0)
var<uniform> scene: SceneUniform;

struct VSOut {
  @builtin(position) pos: vec4f,
  @location(0) uv: vec2f,
  @location(1) nrm: vec3f,
};

@vertex
fn main(
  @location(0) position: vec3f,
  @location(1) normal: vec3f,
  @location(2) uv: vec2f
) -> VSOut {
  var out: VSOut;

  out.pos = scene.mvp * vec4f(position , 1.0);
  out.uv = uv;
  out.nrm = normal;

  return out;
}
