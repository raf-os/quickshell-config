#include "serializableobject.h"

#include <qobject.h>

namespace ns::utils {
SerializableObject::SerializableObject(
    const QString &className, QObject *root, QObject *parent)
    : QObject(parent), m_rootObject(root), m_className(className) {}

SerializableObject::SerializableObject(
    const QString &className, QObject *parent)
    : QObject(parent), m_className(className) {}

QObject *SerializableObject::getRoot() { return m_rootObject; }
QString  SerializableObject::className() const { return m_className; }
} // namespace ns::utils
