#include "stooltip.h"

#include <QtCore>
#include <qcontainerfwd.h>
#include <qlogging.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qproperty.h>
#include <qqmlcomponent.h>
#include <qqmlengine.h>
#include <qqmlinfo.h>
#include <qquickitem.h>
#include <qquickwindow.h>
#include <qtimer.h>
#include <qvariant.h>
#include <qwindow.h>

namespace ns::components {
namespace {
const char *ATTACHED_PROPERTY_NAME = "__ns_tooltip_attached";
}

// TooltipProxyWindow
TooltipProxyWindow::TooltipProxyWindow(QWindow *parent)
    : QQuickWindow(parent) {}

void TooltipProxyWindow::setAlignment(TooltipAlignment::Enum value) {
  if (m_alignment == value) return;

  m_alignment = value;
  reposition();
}
void TooltipProxyWindow::setAnchor(TooltipAnchor::Enum value) {
  if (m_anchor == value) return;

  m_anchor = value;
  reposition();
}

void TooltipProxyWindow::setTooltipContent(QQuickItem *content) {
  if (m_content) {
    m_content->deleteLater();
    m_content = nullptr;
  }

  m_content = content;

  if (content) {
    m_content->setParentItem(contentItem());
    QObject::connect(m_content, &QObject::destroyed, this,
        &TooltipProxyWindow::onContentDestroyed);
    QObject::connect(m_content, &QQuickItem::widthChanged, this,
        &TooltipProxyWindow::reposition);
    QObject::connect(m_content, &QQuickItem::heightChanged, this,
        &TooltipProxyWindow::reposition);
    reposition();
  }
}

void TooltipProxyWindow::setReference(QQuickItem *reference) {
  if (m_reference) {
    QObject::disconnect(m_reference, nullptr, this, nullptr);
  }

  m_reference = reference;

  if (!reference) {
    setVisible(false);
    return;
  }

  QObject::connect(m_reference.get(), &QQuickItem::xChanged, this,
      &TooltipProxyWindow::reposition);
  QObject::connect(m_reference.get(), &QQuickItem::yChanged, this,
      &TooltipProxyWindow::reposition);
  QObject::connect(m_reference.get(), &QQuickItem::widthChanged, this,
      &TooltipProxyWindow::reposition);
  QObject::connect(m_reference.get(), &QQuickItem::heightChanged, this,
      &TooltipProxyWindow::reposition);

  reposition();
}

void TooltipProxyWindow::onContentDestroyed() { m_content = nullptr; }

void TooltipProxyWindow::reposition() {
  if (!m_reference || !m_content) return;

  setX(m_reference->x());
  setY(m_reference->y());
}
// TooltipProxyWindow

// TooltipProvider
TooltipProvider::TooltipProvider(QObject *parent)
    : QObject(parent), m_graceTimer(new QTimer(this)),
      m_contentItem(new QQuickItem()), m_proxyWindow(new TooltipProxyWindow()) {
  m_contentItem->setParent(this);

  m_graceTimer->setInterval(250);
  m_graceTimer->setSingleShot(true);
  QObject::connect(m_graceTimer, &QTimer::timeout, this,
      &TooltipProvider::onGraceTimerTimeout);
}

TooltipProvider::~TooltipProvider() {
  if (m_parentWindow) {
    m_parentWindow->setProperty(ATTACHED_PROPERTY_NAME, QVariant());
  }
}

void TooltipProvider::componentComplete() {
  auto p = parent();
  if (!p) {
    deleteLater();
    return;
  }
  auto item = qobject_cast<QQuickItem *>(p);
  if (!item) {
    deleteLater();
    return;
  }

  m_parentWindow = item->window();
  if (!m_parentWindow) {
    QObject::connect(
        item, &QQuickItem::windowChanged, this, &TooltipProvider::tryAttach);
    return;
  }
  tryAttach(m_parentWindow);
}

void TooltipProvider::tryAttach(QQuickWindow *window) {
  if (m_parentWindow) {
    m_parentWindow->setProperty(ATTACHED_PROPERTY_NAME, {});
  }

  m_parentWindow = window;
  m_proxyWindow->setVisible(false);

  if (!window) return;

  m_proxyWindow->setParent(window);
  m_proxyWindow->setTransientParent(window);
  m_proxyWindow->setFlag(Qt::ToolTip);

  m_contentItem->setParentItem(m_parentWindow->contentItem());

  m_parentWindow->setProperty(
      ATTACHED_PROPERTY_NAME, QVariant::fromValue(this));

  if (m_component) {
    m_component->setParentItem(m_contentItem);
  }

  b_ready = true;
}

bool            TooltipProvider::isActive() const { return m_owner != nullptr; }
bool            TooltipProvider::ready() const { return b_ready.value(); }
QBindable<bool> TooltipProvider::bindableReady() const { return &b_ready; }

QQmlComponent *TooltipProvider::delegate() { return m_delegate; }
void           TooltipProvider::setDelegate(QQmlComponent *delegate) {
  if (m_delegate == delegate) return;

  if (m_component) {
    m_component->deleteLater();
    m_component = nullptr;
  }

  m_delegate = delegate;
  emit delegateChanged();

  QVariantMap initialProps{
      {"text", QVariant::fromValue(QString())}
  };

  auto obj = m_delegate->createWithInitialProperties(
      initialProps, QQmlEngine::contextForObject(m_delegate));
  if (!obj) return;
  auto item = qobject_cast<QQuickItem *>(obj);
  if (!item) {
    obj->deleteLater();
    qmlWarning(this) << "Delegate does not subclass Item!";
    return;
  }

  item->setParent(this);
  item->setParentItem(m_contentItem);

  m_component = item;
  m_component->setVisible(false);
}

void TooltipProvider::requestOwnership(SToolTip *owner) {
  if (!m_delegate || !m_component || !owner) {
    return;
  }

  m_graceTimer->stop();
  m_owner = owner;
  QObject::connect(
      m_owner, &QObject::destroyed, this, &TooltipProvider::onOwnerDestroyed);
  mapContentToOwner();
  m_component->setVisible(true);
  m_proxyWindow->setVisible(true);

  m_component->setProperty("text", owner->bindableText().value());
}
void TooltipProvider::releaseOwnership(SToolTip *self) {
  if (self != m_owner) return;
  m_proxyWindow->setVisible(false);
  if (m_owner) {
    QObject::disconnect(m_owner, nullptr, this, nullptr);

    if (mapContentToOwner()) {
      if (m_component) {
        m_component->setParentItem(m_contentItem);
      }
    }

    m_owner = nullptr;
    m_graceTimer->start();
    return;
  } else {
    m_owner = nullptr;
    onGraceTimerTimeout();
  }
}

void TooltipProvider::onGraceTimerTimeout() {
  if (m_component) {
    m_component->setVisible(false);
    m_component->setParentItem(m_contentItem);
  }
}
void TooltipProvider::onComponentDestroyed() {
  releaseOwnership(m_owner);
  m_component = nullptr;
}
void TooltipProvider::onOwnerDestroyed() { releaseOwnership(m_owner); }
void TooltipProvider::onOwnerTextChanged() {
  if (!m_owner || !m_component) return;
  auto text = m_owner->bindableText().value();
  m_component->setProperty("text", QVariant::fromValue(text));
}

bool TooltipProvider::mapContentToOwner() {
  if (!m_owner || !m_parentWindow) return false;

  auto oItem = m_owner->parentItem();

  if (!oItem) return false;

  auto pos = oItem->mapToItem(m_parentWindow->contentItem(),
      QRectF(0, 0, oItem->bindableWidth().value(),
          oItem->bindableHeight().value()));
  m_contentItem->setX(pos.x());
  m_contentItem->setY(pos.y());
  m_contentItem->setWidth(pos.width());
  m_contentItem->setHeight(pos.height());

  return true;
}
// TooltipProvider

// SToolTip
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

void SToolTip::onActiveChanged() {
  if (!m_valid || !m_provider) return;

  if (b_active.value()) {
    if (m_provider->isActive()) {
      onReadyToRequest();
    } else {
      m_timer->start(bindableDelay().value());
    }
  } else {
    m_provider->releaseOwnership(this);
  }
}

void SToolTip::onWindowChanged(QQuickWindow *window) {
  m_parentWindow = window;
  m_valid        = false;

  if (!window) {
    return;
  }

  auto dProp = window->property(ATTACHED_PROPERTY_NAME);
  if (!dProp.isValid() || !dProp.canConvert<TooltipProvider *>()) return;

  m_provider = dProp.value<TooltipProvider *>();
  if (!m_provider) return;

  m_valid = true;
  onActiveChanged();
}

void SToolTip::onReadyToRequest() {
  if (!m_valid || !m_provider) return;
  m_provider->requestOwnership(this);
}
} // namespace ns::components
