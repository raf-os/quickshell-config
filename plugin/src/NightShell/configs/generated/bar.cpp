#include "bar.h"
#include "serializableobject.h"

namespace ns::configs {
Bar::Bar(const QString &className, QObject *root, QObject *parent) : SerializableObject(className, root, parent) {}

QBindable<int> Bar::bindableHeight() const { return &b_height; }
void Bar::resetHeight() { b_height = 32; }
} // namespace ns::configs