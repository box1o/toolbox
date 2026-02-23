
struct SceneUniform {
mvp: mat4x4f,
       time: f32,
};


@group(0) @binding(1)
  var<uniform> scene: SceneUniform;

@group(0) @binding(1)
  var tex0: texture_2d<f32>;

@group(0) @binding(2)
  var samp0: sampler;

  @fragment
  fn main(
      @location(0) uv: vec2f,
      @location(1) nrm: vec3f
      ) -> @location(0) vec4f {
    // sample texture
    let base = textureSample(tex0, samp0, uv);

    //// tiny bit of lighting so it doesn't look flat
    //let L = normalize(vec3f(-0.45, -1.0, -0.35));
    //let N = normalize(nrm);
    //let ndl = max(dot(N, -L), 0.15);


    return vec4f(base);
  }
