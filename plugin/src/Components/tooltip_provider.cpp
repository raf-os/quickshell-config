#include "tooltip_provider.h"

#include <array>
#include <cmath>

#include <qjsengine.h>
#include <qlogging.h>
#include <qmatrix4x4.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qobjectdefs.h>
#include <qpoint.h>
#include <qproperty.h>
#include <qqml.h>
#include <qqmlcomponent.h>
#include <qqmlengine.h>
#include <qquickitem.h>
#include <qquickwindow.h>
#include <qtimer.h>
#include <qtypes.h>
#include <qvariant.h>
#include <qvectornd.h>

#include "stooltip.h"

namespace ns::components {
namespace {
constexpr std::array<const char *, 1> BINDABLE_PROPS = {"text"};

static TooltipProvider *INSTANCE = nullptr;
} // namespace

TooltipProvider *TooltipProvider::instance(QQmlEngine *engine) {
  if (!INSTANCE) {
    INSTANCE = new TooltipProvider(engine, nullptr);
  }
  return INSTANCE;
}

TooltipProvider *TooltipProvider::create(
    QQmlEngine *engine, QJSEngine * /*unused*/) {
  auto inst = instance(engine);
  if (engine) {
    engine->setObjectOwnership(inst, QQmlEngine::CppOwnership);
  }
  return instance(engine);
}

TooltipProvider::TooltipProvider(QQmlEngine *engine, QObject *parent)
    : QObject(parent), m_graceTimer(new QTimer(this)),
      m_window(new QQuickWindow()) {
  m_window->setFlags(Qt::ToolTip | Qt::WindowTransparentForInput |
                     Qt::WindowDoesNotAcceptFocus);
  m_windowContent = m_window->contentItem();
  m_window->setColor("transparent");

  m_graceTimer->setSingleShot(true);
  m_graceTimer->setInterval(250);
  QObject::connect(m_graceTimer, &QTimer::timeout, this,
      &TooltipProvider::onGraceTimerTimeout);

  if (engine) {
    // Quickshell hot reload tears down the engine. With the way things are set
    // up right now, it's necessary to take the provider out back and kill it
    // old yeller style, so a new one can be set up with the correct QQmlEngine
    // object.
    //
    // If this becomes an issue, consider adding some sort of "canary" object
    // instead that will notify this object whenever it's re-instantiated on the
    // new engine.
    QObject::connect(engine, &QObject::destroyed, this, &QObject::deleteLater);
  }

  m_tooltip = instantiateTooltip(engine);
}

QQuickItem *TooltipProvider::instantiateTooltip(QQmlEngine *engine) {
  if (m_tooltip) return m_tooltip;

  auto _engine = engine;
  if (!_engine) _engine = qmlEngine(this);

  QQmlComponent comp(engine, this);
  comp.loadFromModule(
      QML_MODULE_URI, "TooltipTemplate", QQmlComponent::PreferSynchronous);

  auto obj =
      comp.createWithInitialProperties({}, QQmlEngine::contextForObject(this));

  m_tooltip = qobject_cast<QQuickItem *>(obj);
  if (!m_tooltip) {
    qFatal() << "TooltipProvider: unable to cast tooltip item to QQuickItem.";
  }

  m_tooltip->setParent(this);
  m_tooltip->setParentItem(m_windowContent);

  QObject::connect(m_tooltip, &QObject::destroyed, this,
      &TooltipProvider::onTooltipDestroyed);

  return m_tooltip;
}

TooltipProvider::~TooltipProvider() {
  if (m_window) {
    m_window->deleteLater();
  }
  INSTANCE = nullptr;
}

bool TooltipProvider::isActive() const { return m_owner != nullptr; }

void TooltipProvider::clearOwnerSignals() {
  if (m_owner) {
    QObject::disconnect(m_owner, nullptr, this, nullptr);
    if (m_owner->parentItem()) {
      QObject::disconnect(m_owner->parentItem(), nullptr, this, nullptr);
    }
    m_owner->ownershipLost();
  }
}

void TooltipProvider::requestOwnership(SToolTip *owner) {
  if (!owner || owner == m_owner) return;

  auto ownerItem = owner->parentItem();
  if (!ownerItem) return;

  clearOwnerSignals();

  m_graceTimer->stop();
  m_owner   = owner;
  m_tooltip = instantiateTooltip();

  m_window->setVisible(false);

  QObject::connect(
      owner, &QObject::destroyed, this, &TooltipProvider::onOwnerDestroyed);
  QObject::connect(ownerItem, &QQuickItem::windowChanged, this,
      &TooltipProvider::onOwnerWindowChanged);
  QObject::connect(m_owner, &SToolTip::textChanged, this, [this] {
    m_tooltip->setProperty(
        "text", QVariant::fromValue(m_owner->bindableText().value()));
  });

  m_tooltip->setProperty(
      "text", QVariant::fromValue(m_owner->bindableText().value()));

  m_referenceWindow = ownerItem->window();
  if (!m_referenceWindow) return;

  if (m_window->parent() != m_referenceWindow.get()) {
    m_window->setParent(m_referenceWindow.get());
    m_window->setTransientParent(m_referenceWindow.get());
  }

  mapPosition();
  refit();

  if (m_window->isVisible()) {
    m_windowContent->polish();
  } else {
    m_window->setVisible(true);
  }

  QObject::connect(
      m_owner, &SToolTip::textChanged, this, &TooltipProvider::refit);
}
void TooltipProvider::releaseOwnership(SToolTip *owner) {
  if (owner != m_owner) return;

  clearOwnerSignals();

  m_owner = nullptr;
  m_graceTimer->start();
}

void TooltipProvider::onTooltipDestroyed() { m_tooltip = nullptr; }

void TooltipProvider::onGraceTimerTimeout() { m_window->setVisible(false); }

void TooltipProvider::onOwnerWindowChanged(QQuickWindow *window) {
  m_referenceWindow = window;
  if (window) {
    mapPosition();
    refit();
  } else {
    m_graceTimer->start();
  }
}

void TooltipProvider::onOwnerTextChanged() {}

void TooltipProvider::onOwnerDestroyed() {
  clearOwnerSignals();
  m_owner           = nullptr;
  m_referenceWindow = nullptr;
  m_graceTimer->start();
}

void TooltipProvider::mapPosition() {
  // Pardon the shitty attempts at vector math and linear algebra
  if (!m_owner || !m_referenceWindow) return;

  auto item = m_owner->parentItem();

  if (!item) return;

  const auto anc    = m_owner->bindableAnchor().value();
  const auto align  = m_owner->bindableAlignment().value();
  const auto offset = m_owner->bindableOffset().value();

  const auto horizontal = anc % 2 == 0;

  QMatrix4x4 transform;
  if (horizontal) transform.rotate(90, QVector3D(0.0f, 0.0f, 1.0f));

  // Vector below will only have its X or Y component depending on the
  // orientation
  QVector3D axisVector = transform.mapVector(QVector3D(1.0f, 0.0f, 0.0f)) *
                         QVector3D(item->width(), item->height(), 0.0f);

  switch (align) {
  case TooltipAlignment::Start:  axisVector *= 0.0f; break;
  case TooltipAlignment::Middle: axisVector *= 0.5f; break;
  case TooltipAlignment::End:    break;
  }

  const auto xOffset = anc == TooltipAnchor::Right ? item->width() : 0;
  const auto yOffset = anc == TooltipAnchor::Bottom ? item->height() : 0;

  // Shift it to the correct side
  axisVector += QVector3D(xOffset, yOffset, 0.0f);

  transform = QMatrix4x4();
  transform.rotate((anc * 90) % 360, QVector3D(0.0f, 0.0f, 1.0f));

  // This is the gap between the item and the tooltip
  // Moves the vector away by *offset* in the relative X axis
  // If the anchor is set to right, this means a positive movement on the global
  // X axis. If set to top, a negative movement on the global Y axis.
  const QVector3D offsetVector =
      transform.mapVector(QVector3D(offset, 0.0f, 0.0f));

  const auto itemAbsPos =
      item->mapToItem(m_referenceWindow->contentItem(), 0, 0);

  // Final position
  const QVector3D posVector = QVector3D(static_cast<float>(itemAbsPos.x()),
                                  static_cast<float>(itemAbsPos.y()), 0.0f) +
                              axisVector + offsetVector;

  m_mappedPosition = QVector2D(posVector.x(), posVector.y());
}

void TooltipProvider::refit() {
  // The mapPosition() function is not aware of the dimensions of the tooltip -
  // it simply returns a point close to the desired anchoring position.
  //
  // This one simply calculates the correct start position, as the width and
  // height are always going to be the exact ones from the tooltip QML item.

  if (!m_owner || !m_referenceWindow) return;

  const auto anc   = m_owner->bindableAnchor().value();
  const auto align = m_owner->bindableAlignment().value();

  const auto horizontal = anc % 2 == 0;

  // The X axis is a factor related to alignment: left aligned means nothing's
  // done, middle means we shift by half the axis' side, and right means shift
  // by full length.
  //
  // The Y axis instead anchors the tooltip to it's desired side. For instance,
  // if anchored to the left, the tooltip's RIGHT side is anchored to the
  // object's LEFT - which means the text is aligned right-to-left.
  QVector2D multVec{0.0f, 0.0f};
  switch (align) {
  case TooltipAlignment::Start:  break;
  case TooltipAlignment::Middle: multVec.setX(-0.5f); break;
  case TooltipAlignment::End:    multVec.setX(-1.0f); break;
  }
  switch (anc) {
  case TooltipAnchor::Right:
  case TooltipAnchor::Bottom: multVec.setY(0.0f); break;
  case TooltipAnchor::Left:
  case TooltipAnchor::Top:
    multVec.setY(-1.0f); // this is what does the "opposite" anchoring
    break;
  }

  // Swap X and Y axis if anchors are either left or right
  if (horizontal) {
    multVec = QVector2D(multVec.y(), multVec.x());
  }

  // The positioning on the X and Y axis are always a multiple of the tooltip's
  // respective width and height values. So now we multiply our previous vector
  // with these dimensions.
  auto fpos = multVec * QVector2D(static_cast<float>(m_tooltip->width()),
                            static_cast<float>(m_tooltip->height()));

  m_window->setX(std::floor(m_mappedPosition.x() + fpos.x()));
  m_window->setY(std::floor(m_mappedPosition.y() + fpos.y()));
  m_window->setWidth(std::ceil(m_tooltip->width()));
  m_window->setHeight(std::ceil(m_tooltip->height()));
}

void TooltipProvider::updatePosition() {
  mapPosition();
  refit();
}
} // namespace ns::components
