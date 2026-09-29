#include "chamferrectmaterial.h"

#include <qassert.h>
#include <qsgmaterial.h>
#include <qsgmaterialshader.h>
#include <qsgmaterialtype.h>
#include <qsgrendererinterface.h>

#include "chamferrectshader.h"

namespace ns::components {
ChamferRectMaterial::ChamferRectMaterial() { setFlag(QSGMaterial::Blending); }

QSGMaterialType *ChamferRectMaterial::type() const {
  static QSGMaterialType type;
  return &type;
}

int ChamferRectMaterial::compare(const QSGMaterial *o) const {
  Q_ASSERT(o && type() == o->type());
  const auto *other = static_cast<const ChamferRectMaterial *>(o);
  return other == this ? 0 : 1;
}

QSGMaterialShader *ChamferRectMaterial::createShader(
    QSGRendererInterface::RenderMode) const {
  return new ChamferRectShader;
}
} // namespace ns::components
