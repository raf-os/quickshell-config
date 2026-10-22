#pragma once

#include <qfilesystemwatcher.h>
#include <qobject.h>
#include <qtmetamacros.h>

#include "helpermacros.h"
#include "ns_services_pipewire_export.h"
#include "pwcore.h"
#include "pwregistry.h"

namespace ns::services::pipewire {
class NS_SERVICES_PIPEWIRE_EXPORT Connection : public QObject {
  Q_OBJECT

  AUTO_MEYERS_SINGLETON_DECL(Connection)

public:
  PwRegistry m_registry;

private slots:
  void onFatalError();
  void queueFatalError();
  void onRuntimeDirChanged(const QString & /*unused*/);

private:
  explicit Connection(QObject *parent = nullptr);

  bool                m_fatalErrorQueued = false;
  QFileSystemWatcher *m_socketWatcher    = nullptr;
  QString             m_runtimeDir;
  PwCore              m_core{this};

  bool tryConnect(bool retry);
  void beginReconnect();
  void startSocketWatcher();
  void stopSocketWatcher();
};
} // namespace ns::services::pipewire
