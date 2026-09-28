#version 440

layout(location = 0) in vec4 aVertex;
layout(location = 1) in vec2 aTexCoord;

layout(location = 0) out vec2 vTexCoord;

// uniform block: 120 bytes
layout(std140, binding = 0) uniform buf {
  mat4 qt_Matrix; // offset 0
  float qt_Opacity; // offset 64
  float border_width; // offset 68
  vec4 corners; // offset 72
  vec4 color; // offset 88
  vec4 border_color; // offset 104
} ubuf;

out gl_PerVertex {
  vec4 gl_Position;
};

void main() {
  gl_Position = ubuf.qt_Matrix * aVertex;
  vTexCoord = aTexCoord;
}
