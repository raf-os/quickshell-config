#include "chamferrectmaterial.h"

#include <qsgmaterialshader.h>
#include <qsgrendererinterface.h>

#include "chamferrectshader.h"

namespace ns::components {
QSGMaterialShader *ChamferRectMaterial::createShader(
    QSGRendererInterface::RenderMode) const {
  return new ChamferRectShader;
}
} // namespace ns::components
