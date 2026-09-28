#pragma once

#include <qcolor.h>
#include <qqmlintegration.h>
#include <qquickitem.h>
#include <qsgnode.h>
#include <qtmetamacros.h>
#include <qtypes.h>
#include <qvectornd.h>

namespace ns::components {
class ChamferRectNode : public QSGGeometryNode {
public:
  explicit ChamferRectNode();

  void setRect(const QRectF &bounds);
  void setChamfer(const QVector4D &value);
  void setBorderWidth(qreal value);
  void setColor(const QColor &value);
  void setBorderColor(const QColor &value);
};

class ChamferRect : public QQuickItem {
  Q_OBJECT
  QML_ELEMENT

  Q_PROPERTY(qreal chamfer READ chamfer WRITE setChamfer NOTIFY chamferChanged)
  Q_PROPERTY(qreal borderWidth READ borderWidth WRITE setBorderWidth NOTIFY
          borderWidthChanged)
  Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
  Q_PROPERTY(QColor borderColor READ borderColor WRITE setBorderColor NOTIFY
          borderColorChanged)

public:
  explicit ChamferRect(QQuickItem *parent = nullptr);

  [[nodiscard]] qreal chamfer() const;
  void                setChamfer(qreal value);

  [[nodiscard]] qreal borderWidth() const;
  void                setBorderWidth(qreal value);

  [[nodiscard]] QColor color() const;
  void                 setColor(const QColor &value);

  [[nodiscard]] QColor borderColor() const;
  void                 setBorderColor(const QColor &value);

signals:
  void chamferChanged();
  void borderWidthChanged();
  void colorChanged();
  void borderColorChanged();

protected:
  QSGNode *updatePaintNode(QSGNode *, UpdatePaintNodeData *) override;
  void     geometryChange(
      const QRectF &newGeometry, const QRectF &oldGeometry) override;

private:
  bool m_geometryChanged = true;

  QVector4D m_chamfer        = {0, 0, 0, 0};
  bool      m_chamferChanged = true;

  qreal m_borderWidth        = 0;
  bool  m_borderWidthChanged = true;

  QColor m_color;
  bool   m_colorChanged = true;

  QColor m_borderColor;
  bool   m_borderColorChanged = true;
};
} // namespace ns::components
