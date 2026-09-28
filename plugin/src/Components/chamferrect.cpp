#include "chamferrect.h"

#include <QtCore>
#include <qcolor.h>
#include <qsggeometry.h>
#include <qsgnode.h>
#include <qvectornd.h>

#include "chamferrectmaterial.h"

namespace ns::components {
// ChamferRectNode
ChamferRectNode::ChamferRectNode() {
  auto *m = new ChamferRectMaterial;
  setMaterial(m);
  setFlag(OwnsMaterial, true);

  QSGGeometry *g =
      new QSGGeometry(QSGGeometry::defaultAttributes_TexturedPoint2D(), 4);
  QSGGeometry::updateTexturedRectGeometry(g, QRect(), QRect());
  setGeometry(g);
  setFlag(OwnsGeometry, true);
}

void ChamferRectNode::setRect(const QRectF &bounds) {
  QSGGeometry::updateTexturedRectGeometry(
      geometry(), bounds, QRectF(0, 0, 1, 1));
  markDirty(QSGNode::DirtyGeometry);
}

void ChamferRectNode::setChamfer(const QVector4D &value) {
  auto *m                = static_cast<ChamferRectMaterial *>(material());
  m->uniforms.corners[0] = value.x();
  m->uniforms.corners[1] = value.y();
  m->uniforms.corners[2] = value.z();
  m->uniforms.corners[3] = value.w();
  m->uniforms.dirty      = true;
  markDirty(DirtyMaterial);
}

void ChamferRectNode::setBorderWidth(qreal value) {
  auto *m                  = static_cast<ChamferRectMaterial *>(material());
  m->uniforms.border_width = value;
  m->uniforms.dirty        = true;
  markDirty(DirtyMaterial);
}

void ChamferRectNode::setColor(const QColor &value) {
  auto *m              = static_cast<ChamferRectMaterial *>(material());
  m->uniforms.color[0] = value.redF();
  m->uniforms.color[1] = value.greenF();
  m->uniforms.color[2] = value.blueF();
  m->uniforms.color[3] = value.alphaF();
  m->uniforms.dirty    = true;
  markDirty(DirtyMaterial);
}

void ChamferRectNode::setBorderColor(const QColor &value) {
  auto *m                     = static_cast<ChamferRectMaterial *>(material());
  m->uniforms.border_color[0] = value.redF();
  m->uniforms.border_color[1] = value.greenF();
  m->uniforms.border_color[2] = value.blueF();
  m->uniforms.border_color[3] = value.alphaF();
  m->uniforms.dirty           = true;
  markDirty(DirtyMaterial);
}

// ChamferRect
QSGNode *ChamferRect::updatePaintNode(QSGNode *old, UpdatePaintNodeData *) {
  auto *node = static_cast<ChamferRectNode *>(old);

  if (!node) node = new ChamferRectNode;

  if (m_geometryChanged) node->setRect(boundingRect());
  m_geometryChanged = false;

  if (m_chamferChanged) node->setChamfer(m_chamfer);
  m_chamferChanged = false;

  if (m_colorChanged) node->setColor(m_color);
  m_colorChanged = false;

  if (m_borderWidthChanged) node->setBorderWidth(m_borderWidth);
  m_borderWidthChanged = false;

  if (m_borderColorChanged) node->setBorderColor(m_borderColor);
  m_borderColorChanged = false;

  return node;
}

qreal ChamferRect::chamfer() const { return m_chamfer.x(); }
void  ChamferRect::setChamfer(qreal value) {
  auto      fval = static_cast<float>(value);
  QVector4D newChamfer{fval, fval, fval, fval};

  if (m_chamfer == newChamfer) return;

  m_chamfer        = newChamfer;
  m_chamferChanged = true;
  emit chamferChanged();
  update();
}
} // namespace ns::components
