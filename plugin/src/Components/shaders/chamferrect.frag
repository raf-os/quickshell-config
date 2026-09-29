#version 440

layout(location = 0) in vec2 qt_TexCoord0;

layout(location = 0) out vec4 fragColor;

// uniform block: 128 bytes
layout(std140, binding = 0) uniform buf {
  mat4 qt_Matrix; // offset 0
  float qt_Opacity; // offset 64
  float border_width; // offset 68
  vec2 iResolution; // offset 72
  vec4 corners; // offset 80
  vec4 color; // offset 96
  vec4 border_color; // offset 112
};

// Credits to https://www.shadertoy.com/view/4llXD7
// Original made by Inigo Quilez https://www.shadertoy.com/view/3fc3zs
float sdChamferedBoxFast(in vec2 p, in vec2 b, in vec4 r) {
  vec2 ap = abs(p);
  vec2 q = ap - b;
  float sx = step(0.0, p.x);
  float sy = step(0.0, p.y);
  float rad = mix(mix(r.z, r.x, sx), mix(r.w, r.y, sx), sy);

  float e = q.x + q.y + rad;

  // Inline sdSegment
  float h = clamp(0.5 * (q.y - q.x + rad), 0.0, rad);
  float seg = length(vec2(q.x + h, q.y + rad - h)) * sign(e);

  vec2 m = max(q, 0.0);
  float outd = length(m);
  float ind = min(max(q.x, q.y), 0.0);

  return max(ind + outd, min(max(e, 0.0), seg));
}

void main()
{
  vec2 fragCoord = qt_TexCoord0 * iResolution;
  vec2 p = (2.0 * fragCoord - iResolution);

  float bWidth = (2.0 * border_width);

  float d = sdChamferedBoxFast(p, vec2(iResolution.x, iResolution.y), corners * 2);

  // anti-aliasing amount
  float aa = 1.0;

  float fillMask = 1.0 - smoothstep(-bWidth - aa, -bWidth, d);
  float inner = 1.0 - smoothstep(-aa, 0.0, d);
  vec4 col = mix(border_color, color, fillMask);
  col.a *= inner;

  fragColor = vec4(col.rgb * col.a, col.a) * qt_Opacity;
}
