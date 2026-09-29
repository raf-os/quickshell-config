#pragma once

#include <qcolor.h>
#include <qqmlintegration.h>
#include <qquickitem.h>
#include <qrgb.h>
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
  void setBorderWidth(float value);
  void setColor(const QColor &value);
  void setBorderColor(const QColor &value);
};

class ChamferRect : public QQuickItem {
  Q_OBJECT
  QML_ELEMENT

  Q_PROPERTY(float topLeftChamfer READ topLeftChamfer WRITE setTopLeftChamfer
          NOTIFY chamferChanged)
  Q_PROPERTY(float topRightChamfer READ topRightChamfer WRITE setTopRightChamfer
          NOTIFY chamferChanged)
  Q_PROPERTY(float bottomLeftChamfer READ bottomLeftChamfer WRITE
          setBottomLeftChamfer NOTIFY chamferChanged)
  Q_PROPERTY(float bottomRightChamfer READ bottomRightChamfer WRITE
          setBottomRightChamfer NOTIFY chamferChanged)
  Q_PROPERTY(float chamfer READ chamfer WRITE setChamfer NOTIFY chamferChanged)
  Q_PROPERTY(float borderWidth READ borderWidth WRITE setBorderWidth NOTIFY
          borderWidthChanged)
  Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
  Q_PROPERTY(QColor borderColor READ borderColor WRITE setBorderColor NOTIFY
          borderColorChanged)

public:
  explicit ChamferRect(QQuickItem *parent = nullptr);

  [[nodiscard]] float chamfer() const;
  void                setChamfer(float value);

  [[nodiscard]] float topLeftChamfer() const;
  void                setTopLeftChamfer(float value);
  [[nodiscard]] float topRightChamfer() const;
  void                setTopRightChamfer(float value);
  [[nodiscard]] float bottomLeftChamfer() const;
  void                setBottomLeftChamfer(float value);
  [[nodiscard]] float bottomRightChamfer() const;
  void                setBottomRightChamfer(float value);

  [[nodiscard]] float borderWidth() const;
  void                setBorderWidth(float value);

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

  float m_borderWidth        = 0;
  bool  m_borderWidthChanged = true;

  QColor m_color        = QRgb(0x000000ff);
  bool   m_colorChanged = true;

  QColor m_borderColor        = QRgb(0xffffffff);
  bool   m_borderColorChanged = true;

  float cappedChamfer(float from);
};
} // namespace ns::components
