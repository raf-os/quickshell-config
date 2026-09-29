#include "chamferrectshader.h"

#include <cstring>

#include <qassert.h>
#include <qmatrix4x4.h>
#include <qsgmaterial.h>
#include <qsgmaterialshader.h>
#include <qstringview.h>

#include "chamferrectmaterial.h"

namespace ns::components {
ChamferRectShader::ChamferRectShader() {
  setShaderFileName(
      VertexStage, ":/Nightshell/Components/shaders/chamferrect.vert.qsb");
  setShaderFileName(
      FragmentStage, ":/Nightshell/Components/shaders/chamferrect.frag.qsb");
}

bool ChamferRectShader::updateUniformData(
    RenderState &state, QSGMaterial *newMaterial, QSGMaterial *oldMaterial) {
  bool        changed = false;
  QByteArray *buf     = state.uniformData();
  Q_ASSERT(buf->size() >= 120);

  if (state.isMatrixDirty()) {
    const QMatrix4x4 m = state.combinedMatrix();
    memcpy(buf->data(), m.constData(), 64);
    changed = true;
  }

  if (state.isOpacityDirty()) {
    const float opacity = state.opacity();
    memcpy(buf->data() + 64, &opacity, 4);
    changed = true;
  }

  auto *customMaterial = static_cast<ChamferRectMaterial *>(newMaterial);
  if (oldMaterial != newMaterial || customMaterial->uniforms.dirty) {
    memcpy(buf->data() + 68, &customMaterial->uniforms.border_width, 4);
    memcpy(buf->data() + 72, &customMaterial->uniforms.iResolution, 8);
    memcpy(buf->data() + 80, &customMaterial->uniforms.corners, 16);
    memcpy(buf->data() + 96, &customMaterial->uniforms.color, 16);
    memcpy(buf->data() + 112, &customMaterial->uniforms.border_color, 16);
    customMaterial->uniforms.dirty = false;
    changed                        = true;
  }

  return changed;
}
} // namespace ns::components
