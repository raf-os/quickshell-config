#include "stooltip.h"

#include <QtCore>
#include <qcontainerfwd.h>
#include <qlogging.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qproperty.h>
#include <qqml.h>
#include <qqmlcomponent.h>
#include <qqmlengine.h>
#include <qqmlinfo.h>
#include <qquickitem.h>
#include <qquickwindow.h>
#include <qtimer.h>
#include <qvariant.h>
#include <qwindow.h>

#include "tooltip_provider.h"

namespace ns::components {
SToolTip::SToolTip(QObject *parent)
    : QObject(parent), m_timer(new QTimer(this)) {
  QObject::connect(
      this, &SToolTip::activeChanged, this, &SToolTip::onActiveChanged);
  m_timer->setSingleShot(true);
  QObject::connect(
      m_timer, &QTimer::timeout, this, &SToolTip::onReadyToRequest);
}

void SToolTip::componentComplete() {
  auto parentItem = this->parentItem();

  if (!parentItem) {
    qmlWarning(this) << "SToolTip must be a child of a QQuickItem!";
    return;
  }

  QObject::connect(
      parentItem, &QQuickItem::windowChanged, this, &SToolTip::onWindowChanged);

  auto window = parentItem->window();
  if (!window) {
    return;
  }

  onWindowChanged(window);
}

void SToolTip::ownershipLost() {}

QQuickItem *SToolTip::parentItem() {
  return parent() ? qobject_cast<QQuickItem *>(parent()) : nullptr;
}

QBindable<bool>                SToolTip::bindableActive() { return &b_active; }
QBindable<QString>             SToolTip::bindableText() { return &b_text; }
QBindable<int>                 SToolTip::bindableDelay() { return &b_delay; }
QBindable<TooltipAnchor::Enum> SToolTip::bindableAnchor() { return &b_anchor; }
QBindable<TooltipAlignment::Enum> SToolTip::bindableAlignment() {
  return &b_alignment;
}
QBindable<qreal> SToolTip::bindableOffset() { return &b_offset; }

void SToolTip::onActiveChanged() {
  auto provider = TooltipProvider::instance(qmlEngine(this));
  if (!m_valid || !provider) return;

  if (b_active.value()) {
    if (provider->isActive()) {
      onReadyToRequest();
    } else {
      m_timer->start(bindableDelay().value());
    }
  } else {
    m_timer->stop();
    provider->releaseOwnership(this);
  }
}

void SToolTip::onWindowChanged(QQuickWindow *window) {
  m_parentWindow = window;
  m_valid        = false;

  if (!window) {
    return;
  }

  m_valid = true;
  onActiveChanged();
}

void SToolTip::onReadyToRequest() {
  auto provider = TooltipProvider::instance(qmlEngine(this));
  if (!m_valid || !provider) return;
  provider->requestOwnership(this);
}
} // namespace ns::components
