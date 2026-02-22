struct SceneUniform {
  mvp: mat4x4f,
};

@group(0) @binding(0)
var<uniform> scene: SceneUniform;

struct VSOut {
  @builtin(position) pos: vec4f,
  @location(0) nrm: vec3f,
};

@vertex
fn main(
  @location(0) position: vec3f,
  @location(1) normal: vec3f
) -> VSOut {
  var out: VSOut;
  out.pos = scene.mvp * vec4f(position, 1.0);
  out.nrm = normal;
  return out;
}
