#include "chamferrect.h"

#include <algorithm>

#include <QtCore>
#include <qcolor.h>
#include <qnumeric.h>
#include <qquickitem.h>
#include <qsggeometry.h>
#include <qsgnode.h>
#include <qtypes.h>
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
  auto *m                    = static_cast<ChamferRectMaterial *>(material());
  m->uniforms.iResolution[0] = bounds.width();
  m->uniforms.iResolution[1] = bounds.height();
  m->uniforms.dirty          = true;
  markDirty(QSGNode::DirtyGeometry | DirtyMaterial);
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

void ChamferRectNode::setBorderWidth(float value) {
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
ChamferRect::ChamferRect(QQuickItem *parent) : QQuickItem(parent) {
  setFlag(ItemHasContents, true);
}

QSGNode *ChamferRect::updatePaintNode(QSGNode *old, UpdatePaintNodeData *) {
  auto *node = static_cast<ChamferRectNode *>(old);

  if (!node) node = new ChamferRectNode;

  if (m_geometryChanged) {
    node->setRect(boundingRect());
  }
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

void ChamferRect::geometryChange(
    const QRectF &newGeometry, const QRectF &oldGeometry) {
  m_geometryChanged = true;
  update();
  QQuickItem::geometryChange(newGeometry, oldGeometry);
}

float ChamferRect::cappedChamfer(float from) {
  float value = static_cast<float>(std::min({width(), height()})) / 2;
  if (value > from) return from;
  return value;
}

float ChamferRect::chamfer() const { return m_chamfer.x(); }
void  ChamferRect::setChamfer(float value) {
  QVector4D newChamfer{value, value, value, value};

  if (m_chamfer == newChamfer) return;

  m_chamfer        = newChamfer;
  m_chamferChanged = true;
  emit chamferChanged();
  update();
}

#define CHAMFER_CORNER(Coordinate, CoordinateUpper, Getter, Setter)            \
  float ChamferRect::Getter() const { return m_chamfer.Coordinate(); }         \
  void  ChamferRect::Setter(float value) {                                     \
    auto val = cappedChamfer(value);                                           \
    if (m_chamfer.Coordinate() == val) return;                                 \
    m_chamfer.set##CoordinateUpper(val);                                       \
    m_chamferChanged = true;                                                   \
    emit chamferChanged();                                                     \
    update();                                                                  \
  }

CHAMFER_CORNER(x, X, topRightChamfer, setTopRightChamfer)
CHAMFER_CORNER(y, Y, bottomRightChamfer, setBottomRightChamfer)
CHAMFER_CORNER(z, Z, topLeftChamfer, setTopLeftChamfer)
CHAMFER_CORNER(w, W, bottomLeftChamfer, setBottomLeftChamfer)
#undef CHAMFER_CORNER

float ChamferRect::borderWidth() const { return m_borderWidth; }
void  ChamferRect::setBorderWidth(float value) {
  if (qFuzzyCompare(m_borderWidth, value)) return;

  m_borderWidth        = value;
  m_borderWidthChanged = true;
  emit borderWidthChanged();
  update();
}

QColor ChamferRect::color() const { return m_color; }
void   ChamferRect::setColor(const QColor &value) {
  if (m_color == value) return;

  m_color        = value;
  m_colorChanged = true;
  emit colorChanged();
  update();
}

QColor ChamferRect::borderColor() const { return m_borderColor; }
void   ChamferRect::setBorderColor(const QColor &value) {
  if (m_borderColor == value) return;

  m_borderColor        = value;
  m_borderColorChanged = true;
  emit borderColorChanged();
  update();
}
} // namespace ns::components
