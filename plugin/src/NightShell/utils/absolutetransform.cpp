#include "absolutetransform.h"

#include <QtQuick/qquickitem.h>
#include <qlist.h>
#include <qlogging.h>
#include <qobject.h>
#include <qproperty.h>
#include <qqmlinfo.h>
#include <qvectornd.h>

namespace ns::utils {
AbsoluteTransform::AbsoluteTransform(QObject *parent) : QObject(parent) {
  b_xEnd.setBinding([this] { return b_x.value() + b_width.value(); });
  b_yEnd.setBinding([this] { return b_y.value() + b_height.value(); });
}

void AbsoluteTransform::componentComplete() {
  auto qqParent = qobject_cast<QQuickItem *>(QObject::parent());
  if (qqParent) {
    b_width.setBinding([this, qqParent] {
      return qqParent->bindableWidth().value() + b_padding.value().x();
    });
    b_height.setBinding([this, qqParent] {
      return qqParent->bindableHeight().value() + b_padding.value().y();
    });
  }
}

QBindable<qreal> AbsoluteTransform::bindableX() const { return &b_x; }
QBindable<qreal> AbsoluteTransform::bindableY() const { return &b_y; }
QBindable<qreal> AbsoluteTransform::bindableWidth() const { return &b_width; }
QBindable<qreal> AbsoluteTransform::bindableHeight() const { return &b_height; }

QQuickItem *AbsoluteTransform::target() { return m_target; }
void        AbsoluteTransform::setTarget(QQuickItem *target) {
  if (target == m_target) return;

  if (m_target) {
    disconnectChain();
  }

  if (target) {
    QList<QQuickItem *> chainList;

    QQuickItem *recursiveParent = qobject_cast<QQuickItem *>(QObject::parent());
    while (recursiveParent != nullptr && recursiveParent != target) {
      chainList.append(recursiveParent);

      recursiveParent = qobject_cast<QQuickItem *>(recursiveParent->parent());
    }
    if (!recursiveParent) {
      qmlWarning(this) << "Object is not a descendant of target.";
      return;
    }

    m_targetChain = chainList;

    for (auto &node : chainList) {
      if (node == target) continue;

      QObject::connect(
          node, &QObject::destroyed, this, &AbsoluteTransform::onChainBroken);
      QObject::connect(
          node, &QQuickItem::xChanged, this, &AbsoluteTransform::updateX);
      QObject::connect(
          node, &QQuickItem::yChanged, this, &AbsoluteTransform::updateY);
    }

    updateX();
    updateY();

    QObject::connect(target, &QObject::destroyed, this,
        &AbsoluteTransform::onTargetDestroyed);
  } else {
    disconnectChain();
    resetPosition();
  }

  m_target = target;
  emit targetChanged();
}

void AbsoluteTransform::disconnectChain() {
  for (auto &item : m_targetChain) {
    QObject::disconnect(item, nullptr, this, nullptr);
  }
  m_targetChain.clear();
}

void AbsoluteTransform::onTargetDestroyed() {
  resetPosition();
  disconnectChain();
  m_target = nullptr;
  emit targetChanged();
}

void AbsoluteTransform::onChainBroken() { onTargetDestroyed(); }

void AbsoluteTransform::updateX() {
  qreal sum = 0;
  for (const auto &node : m_targetChain) {
    sum += node->bindableX().value();
  }
  b_x = sum;
}

void AbsoluteTransform::updateY() {
  qreal sum = 0;
  for (const auto &node : m_targetChain) {
    sum += node->bindableY().value();
  }
  b_y = sum;
}

void AbsoluteTransform::resetPosition() {
  QScopedPropertyUpdateGroup scope;
  b_x      = 0;
  b_y      = 0;
  b_width  = 0;
  b_height = 0;
}
} // namespace ns::utils
