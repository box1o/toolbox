struct SceneUniforms {
  mvp : mat4x4f,
  model : mat4x4f,
  lightDir : vec4f,
  lightColor : vec4f,
  baseColor : vec4f,
};

@group(0) @binding(0) var<uniform> scene : SceneUniforms;

struct VSIn {
  @location(0) position : vec3f,
  @location(1) normal : vec3f,
};

struct VSOut {
  @builtin(position) pos : vec4f,
  @location(0) worldNormal : vec3f,
};

@vertex
fn main(in : VSIn) -> VSOut {
  var out : VSOut;
  let clip = scene.mvp * vec4f(in.position, 1.0);

  out.pos = vec4f(clip.xy, clip.z * 0.5 + clip.w * 0.5, clip.w);
  out.worldNormal = normalize((scene.model * vec4f(in.normal, 0.0)).xyz);
  return out;
}
