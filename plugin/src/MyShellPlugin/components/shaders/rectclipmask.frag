#version 440
layout(location = 0) in vec2 qt_TexCoord0;
layout(location = 0) out vec4 fragColor;

layout(std140, binding = 0) uniform buf {
  mat4 qt_Matrix;
  float qt_Opacity;
};

layout(binding = 1) uniform sampler2D rect;
layout(binding = 2) uniform sampler2D content;

void main() {
  vec4 contentColor = texture(content, qt_TexCoord0.xy);
  vec4 rectColor = texture(rect, qt_TexCoord0.xy);

  vec4 col = contentColor * rectColor.a;

  fragColor = col * qt_Opacity;
}
