struct VSOut {
  @builtin(position) pos : vec4f,
  @location(0) color : vec3f,
};

@vertex
fn main(@location(0) inPos : vec3f) -> VSOut {
  var o : VSOut;
  o.pos = vec4f(inPos, 1.0);
  o.color = vec3f(1.0, 0.6, 0.2);
  return o;
}
