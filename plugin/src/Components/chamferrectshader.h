#pragma once

#include <qsgmaterialshader.h>

namespace ns::components {
class ChamferRectShader : public QSGMaterialShader {
public:
  explicit ChamferRectShader();
  bool updateUniformData(RenderState &state, QSGMaterial *newMaterial,
      QSGMaterial *oldMaterial) override;
};
} // namespace ns::components
