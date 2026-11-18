#include "debounced_properties.h"

#include <qmetaobject.h>
#include <qobject.h>
#include <qvariant.h>

namespace ns::components {
DebouncedPropertyViewer::DebouncedPropertyViewer(QObject *parent)
    : QObject(parent) {}

QObject *DebouncedPropertyViewer::ref() { return m_reference; }
void     DebouncedPropertyViewer::setRef(QObject *value) {
  if (value == m_reference.get()) return;

  if (m_reference) {
    QObject::disconnect(m_reference, nullptr, this, nullptr);
  }

  m_reference = value;
  m_trackedPropertyNames.clear();
  emit refChanged();

  if (m_reference) {
    QObject::connect(m_reference, &QObject::destroyed, this,
        &DebouncedPropertyViewer::onReferenceDestroyed);

    auto refMeta = m_reference->metaObject();
    for (auto i = refMeta->propertyOffset(); i < refMeta->propertyCount(); i++)
    {
      auto rProp = refMeta->property(i);
      if (!rProp.isWritable()) continue;

      m_trackedPropertyNames.append(QString::fromUtf8(rProp.name()));
    }

    syncObjects();
  }
}

QObject *DebouncedPropertyViewer::target() { return m_target; }
void     DebouncedPropertyViewer::setTarget(QObject *value) {
  if (m_target == value) return;

  if (m_target) {
    QObject::disconnect(m_target, nullptr, this, nullptr);
  }

  m_target = value;
  emit targetChanged();

  if (m_target) {
    QObject::connect(m_target, &QObject::destroyed, this,
        &DebouncedPropertyViewer::onTargetDestroyed);

    syncObjects();
  }
}

void DebouncedPropertyViewer::onTargetDestroyed() {}

void DebouncedPropertyViewer::onReferenceDestroyed() {}

void DebouncedPropertyViewer::propertyChanged(const char *property) {}

void DebouncedPropertyViewer::syncObjects() {
  if (!m_reference || !m_target) return;

  auto tMeta = m_target->metaObject();
  for (auto i = tMeta->propertyOffset(); i < tMeta->propertyCount(); i++) {
    auto mProp = tMeta->property(i);
    if (!mProp.isReadable() || !mProp.hasNotifySignal()) continue;

    if (!m_trackedPropertyNames.contains(QString::fromUtf8(mProp.name())))
      continue;

    auto rMeta        = m_reference->metaObject();
    auto referenceIdx = rMeta->indexOfProperty(mProp.name());
    if (referenceIdx == -1) continue;

    auto rProp = rMeta->property(referenceIdx);
    if (!rProp.isWritable()) continue;

    auto targetIdx = i;

    rProp.write(m_reference, mProp.read(m_target));

    tMeta->connect(
        m_target, mProp.notifySignal(), this, [targetIdx, referenceIdx, this] {
          auto rMeta = m_reference->metaObject();
          auto tMeta = m_target->metaObject();

          auto rProp = rMeta->property(referenceIdx);
          auto tProp = tMeta->property(targetIdx);

          rProp.write(m_reference, tProp.read(m_target));
        });
  }
}
} // namespace ns::components
