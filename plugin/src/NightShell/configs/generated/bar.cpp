#include "bar.h"
#include "serializableobject.h"

namespace ns::configs {
BarConfig::BarConfig(const QString &className, QObject *root, QObject *parent) : SerializableObject(className, root, parent) {}

QBindable<int> BarConfig::bindableHeight() const { return &b_height; }
void BarConfig::resetHeight() { b_height = 32; }
} // namespace ns::configs