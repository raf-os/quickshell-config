#pragma once

#include <qobject.h>
#include <qqmlintegration.h>
#include <qtmetamacros.h>

#include "helpermacros.h"
#include "mpris_service_watcher.h"

namespace ns::mpris {
class MprisSingletonQML : public QObject {
  Q_OBJECT
  QML_NAMED_ELEMENT(Mpris)
  QML_SINGLETON

  Q_PROPERTY(ns::mpris::MprisPlayerListModel *model READ model CONSTANT)

  AUTO_MEYERS_SINGLETON_QML_DECL(MprisSingletonQML)

public:
  [[nodiscard]] Q_INVOKABLE bool      isValid() const;
  [[nodiscard]] MprisPlayerListModel *model();

private:
  explicit MprisSingletonQML(QObject *parent = nullptr);

  MprisServiceWatcher *m_watcher;
};
} // namespace ns::mpris
