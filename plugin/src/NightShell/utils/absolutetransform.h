#pragma once

#include <qlist.h>
#include <qobject.h>
#include <qproperty.h>
#include <qqmlintegration.h>
#include <qqmlparserstatus.h>
#include <qquickitem.h>
#include <qtmetamacros.h>
#include <qtypes.h>
#include <qvectornd.h>

#include "helpermacros.h"

namespace ns::utils {
class AbsoluteTransform : public QObject, public QQmlParserStatus {
  Q_OBJECT
  QML_ELEMENT
  Q_INTERFACES(QQmlParserStatus)

  Q_PROPERTY(qreal x READ default NOTIFY xChanged BINDABLE bindableX)
  Q_PROPERTY(qreal y READ default NOTIFY yChanged BINDABLE bindableY)
  Q_PROPERTY(
      qreal width READ default NOTIFY widthChanged BINDABLE bindableWidth)
  Q_PROPERTY(
      qreal height READ default NOTIFY heightChanged BINDABLE bindableHeight)

  Q_PROPERTY(QQuickItem *targetAncestor READ target WRITE setTarget NOTIFY
          targetChanged)

  Q_PROPERTY(QVector2D padding READ default WRITE default NOTIFY paddingChanged
          BINDABLE bindablePadding)

#define B(Type, Name) AUTO_BINDABLE_WRITABLE(AbsoluteTransform, Type, Name)
  B(qreal, xEnd)
  B(qreal, yEnd)
#undef B

public:
  explicit AbsoluteTransform(QObject *parent = nullptr);

  enum BindingRefresh : quint8 {
    XBindings = 0,
    YBindings = 1,
  };

  void classBegin() override {}
  void componentComplete() override;

  [[nodiscard]] QBindable<qreal>     bindableX() const;
  [[nodiscard]] QBindable<qreal>     bindableY() const;
  [[nodiscard]] QBindable<qreal>     bindableWidth() const;
  [[nodiscard]] QBindable<qreal>     bindableHeight() const;
  [[nodiscard]] QBindable<QVector2D> bindablePadding() { return &b_padding; }

  [[nodiscard]] QQuickItem *target();
  void                      setTarget(QQuickItem *target);

signals:
  void xChanged();
  void yChanged();
  void widthChanged();
  void heightChanged();
  void targetChanged();
  void paddingChanged();

private slots:
  void  onTargetDestroyed();
  void  onChainBroken();
  qreal calculatePositions(BindingRefresh type);

private:
  QQuickItem         *m_target = nullptr;
  QList<QQuickItem *> m_targetChain;

  void disconnectChain();
  void resetPosition();

  Q_OBJECT_BINDABLE_PROPERTY(
      AbsoluteTransform, qreal, b_x, &AbsoluteTransform::xChanged)
  Q_OBJECT_BINDABLE_PROPERTY(
      AbsoluteTransform, qreal, b_y, &AbsoluteTransform::yChanged)
  Q_OBJECT_BINDABLE_PROPERTY(
      AbsoluteTransform, qreal, b_width, &AbsoluteTransform::widthChanged)
  Q_OBJECT_BINDABLE_PROPERTY(
      AbsoluteTransform, qreal, b_height, &AbsoluteTransform::heightChanged)
  Q_OBJECT_BINDABLE_PROPERTY(AbsoluteTransform, QVector2D, b_padding,
      &AbsoluteTransform::paddingChanged)
};
} // namespace ns::utils
