#pragma once

#include <qlist.h>
#include <qobject.h>
#include <qqmlintegration.h>
#include <qtclasshelpermacros.h>
#include <qtmetamacros.h>
#include <qtypes.h>

#include "helpermacros.h"
#include "ns_services_pipewire_export.h"

namespace ns::services::pipewire::qml {
class NS_SERVICES_PIPEWIRE_EXPORT PwTracker : public QObject {
  Q_OBJECT
  QML_ELEMENT

public:
  explicit PwTracker(QObject *parent = nullptr);
};

class NS_SERVICES_PIPEWIRE_EXPORT Pipewire : public QObject {
  Q_OBJECT
  QML_ELEMENT
  QML_SINGLETON

  AUTO_MEYERS_SINGLETON_QML_DECL(Pipewire)

signals:
  void sinksChanged();
  void sourcesChanged();
  void linksChanged();

private:
  explicit Pipewire(QObject *parent = nullptr);

  QList<QObject *> m_sinks;
  QList<QObject *> m_sources;
  QList<QObject *> m_links;
};
} // namespace ns::services::pipewire::qml
