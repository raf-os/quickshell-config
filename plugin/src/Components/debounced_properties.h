#pragma once

#include <qcontainerfwd.h>
#include <qobject.h>
#include <qpointer.h>
#include <qqmlintegration.h>
#include <qtmetamacros.h>

namespace ns::components {
// This is a sort of "debouncer" - meant to be used specifically within QML, for
// objects that have exit animations and read their properties from a QObject
// pointer.
class DebouncedPropertyViewer : public QObject {
  Q_OBJECT
  QML_ELEMENT

  // "Buffer" object, will try to match its dynamic properties to the "target"
  // below
  // WARNING: All values are copied!
  Q_PROPERTY(QObject *ref READ ref WRITE setRef NOTIFY refChanged)
  Q_PROPERTY(QObject *target READ target WRITE setTarget NOTIFY targetChanged)

public:
  explicit DebouncedPropertyViewer(QObject *parent = nullptr);

  [[nodiscard]] QObject *ref();
  void                   setRef(QObject *value);

  [[nodiscard]] QObject *target();
  void                   setTarget(QObject *value);

signals:
  void refChanged();
  void targetChanged();

private slots:
  void onReferenceDestroyed();
  void onTargetDestroyed();

private:
  QPointer<QObject> m_target    = nullptr;
  QPointer<QObject> m_reference = nullptr;

  QStringList m_trackedPropertyNames;

  void propertyChanged(const char *property);
  void syncObjects();
};
} // namespace ns::components
