#include "object_repeater.h"

#include <QQmlEngine>
#include <QVariant>
#include <QtLogging>

namespace ns {
ObjectRepeaterBase::ObjectRepeaterBase(QObject *parent) : QObject(parent) {}

void ObjectRepeaterBase::componentComplete() {}

QQmlComponent *ObjectRepeaterBase::delegate() { return m_delegate; }

ScreenObjectRepeater::ScreenObjectRepeater(QObject *parent)
    : ObjectRepeaterBase(parent) {}

void ScreenObjectRepeater::updateInstances() {
  if (!delegate()) {
    return;
  }

  // First, remove objects that are no longer needed
  for (auto [key, value] : m_instances.asKeyValueRange()) {
    if (!m_model.contains(key)) {
      value->deleteLater();
      m_instances.remove(key);
    }
  }

  // Now create new ones as necessary
  for (const auto &screen : m_model) {
    if (m_instances.contains(screen)) continue;

    QVariantMap ctx{
        {"modelData", QVariant::fromValue(screen)},
    };

    auto *instance = delegate()->createWithInitialProperties(
        ctx, QQmlEngine::contextForObject(delegate()));

    if (!instance) {
      qWarning() << delegate()->errorString();
      continue;
    }

    QQmlEngine::setObjectOwnership(instance, QQmlEngine::CppOwnership);
    instance->setParent(this);

    m_instances.insert(screen, instance);
  }
}

QList<QScreen *> ScreenObjectRepeater::screens() const { return m_model; }
void ScreenObjectRepeater::setScreens(const QList<QScreen *> &value) {
  if (m_model == value) return;

  m_model = value;
  updateInstances();

  emit screensChanged();
}
} // namespace ns
