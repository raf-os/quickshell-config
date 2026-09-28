#pragma once

#include <qsgmaterial.h>
#include <qsgmaterialshader.h>
#include <qsgmaterialtype.h>
#include <qsgnode.h>
#include <qsgrendererinterface.h>

namespace ns::components {
class ChamferRectMaterial : public QSGMaterial {
public:
  explicit ChamferRectMaterial();
  QSGMaterialType *type() const override;
  int              compare(const QSGMaterial *other) const override;

  QSGMaterialShader *createShader(
      QSGRendererInterface::RenderMode) const override;

  struct {
    float border_width;
    float corners[4];
    float color[4];
    float border_color[4];
    bool  dirty;
  } uniforms;
};
} // namespace ns::components
