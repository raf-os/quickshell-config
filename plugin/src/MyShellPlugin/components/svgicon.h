#pragma once

#include <qcolor.h>
#include <qobject.h>
#include <qpainter.h>
#include <qqmlintegration.h>
#include <qquickitem.h>
#include <qquickpainteditem.h>
#include <qstringview.h>
#include <qsvgrenderer.h>
#include <qtmetamacros.h>
#include <qtypes.h>

namespace ns {
namespace components {
/*
 * SVGIcon
 *
 * Provides a shell SVG icon. The icon is rasterized on the CPU, so animating
 * dimensions is likely to cause performance issues. If needed, animate scale
 * instead.
 */
class SVGIcon : public QQuickPaintedItem {
  Q_OBJECT
  QML_ELEMENT
  Q_PROPERTY(
      QString iconName READ iconName WRITE setIconName NOTIFY iconNameChanged)
  Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
  Q_PROPERTY(qreal size READ size WRITE setSize NOTIFY sizeChanged)

public:
  explicit SVGIcon(QQuickItem *parent = nullptr);
  void paint(QPainter *painter) override;

  [[nodiscard]] QString iconName() const;
  void setIconName(const QString &value);

  [[nodiscard]] QColor color() const;
  void setColor(const QColor &value);

  [[nodiscard]] qreal size() const;
  void setSize(const qreal &value);

  Q_SLOT void triggerUpdate();

signals:
  void iconNameChanged();
  void colorChanged();
  void sizeChanged();

private:
  QSvgRenderer *m_renderer = nullptr;
  QString m_iconDir = QString::fromUtf8(DATADIR_DEFAULT_PATH) + "/icons";
  QString m_iconName;
  QColor m_color;
  qreal m_size;

  void processXmlData(const QString &source);
};
} // namespace components
} // namespace ns
