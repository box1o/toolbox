// fs.wgsl
@fragment
fn main(@location(0) nrm: vec3f) -> @location(0) vec4f {
  return vec4f(nrm, 1.0);
}
