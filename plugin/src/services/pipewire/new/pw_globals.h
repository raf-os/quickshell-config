#pragma once

#include <memory>

#include <qfilesystemwatcher.h>
#include <qobject.h>
#include <qqmlintegration.h>
#include <qsocketnotifier.h>
#include <qtclasshelpermacros.h>
#include <qtmetamacros.h>

#include "helpermacros.h"

namespace ns::pipewire {
class Core;
class Registry;
class DefaultTracker;

class Globals : public QObject {
  Q_OBJECT
  Q_DISABLE_COPY_MOVE(Globals)

  AUTO_MEYERS_SINGLETON_DECL(Globals)

public:
  [[nodiscard]] Core           *core();
  [[nodiscard]] Registry       *registry();
  [[nodiscard]] DefaultTracker *defaults();

private slots:
  void queueFatalError();
  void onFatalError();
  void onRuntimeDirChanged(const QString & /*unused*/);

private:
  explicit Globals(QObject *parent = nullptr);

  bool                m_fatalErrorQueued = false;
  QString             m_runtimeDir;
  QFileSystemWatcher *m_socketWatcher = nullptr;

  Registry       *m_registry = nullptr;
  DefaultTracker *m_defaults = nullptr;
  Core           *m_core     = nullptr;

  bool tryConnect(bool retry);
  void beginReconnect();

  void startSocketWatcher();
  void stopSocketWatcher();
};

class PwQmlInitializer : public QObject {
  Q_OBJECT
  QML_ELEMENT

public:
  explicit PwQmlInitializer(QObject *parent = nullptr) : QObject(parent) {
    Globals::instance();
  }
};
} // namespace ns::pipewire
