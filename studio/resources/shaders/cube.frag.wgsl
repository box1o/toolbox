struct SceneUniforms {
  mvp : mat4x4f,
  model : mat4x4f,
  lightDir : vec4f,
  lightColor : vec4f,
  baseColor : vec4f,
};

@group(0) @binding(0) var<uniform> scene : SceneUniforms;

@fragment
fn main(@location(0) worldNormal : vec3f) -> @location(0) vec4f {
  let n = normalize(worldNormal);
  let l = normalize(-scene.lightDir.xyz);
  let ndotl = max(dot(n, l), 0.0);

  let ambient = 0.18;
  let diffuse = ambient + 0.82 * ndotl;
  let lit = scene.baseColor.xyz * scene.lightColor.xyz * diffuse;

  return vec4f(lit, scene.baseColor.w);
}
