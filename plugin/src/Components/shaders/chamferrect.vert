#version 440

layout(location = 0) in vec4 qt_Vertex;
layout(location = 1) in vec2 qt_MultiTexCoord0;

layout(location = 0) out vec2 qt_TexCoord0;

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

out gl_PerVertex {
  vec4 gl_Position;
};

void main() {
  gl_Position = qt_Matrix * qt_Vertex;
  qt_TexCoord0 = qt_MultiTexCoord0;
}
