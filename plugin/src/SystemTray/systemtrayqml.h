#pragma once

#include <qobject.h>
#include <qqmlintegration.h>
#include <qtmetamacros.h>

#include "objectmodel.h"
#include "statusnotifieritem.h"

namespace ns::systemtray {
class SystemTrayQML : public QObject {
  Q_OBJECT
  QML_ELEMENT
  QML_SINGLETON

  Q_PROPERTY(UntypedObjectModel *items READ items CONSTANT)

public:
  explicit SystemTrayQML(QObject *parent = nullptr);

  [[nodiscard]] ObjectModel<StatusNotifierItem> *items();

private slots:
  void onItemRegistered(StatusNotifierItem *item);
  void onItemUnregistered(StatusNotifierItem *item);

private:
  ObjectModel<StatusNotifierItem> m_items{this};

  static bool sortCompare(StatusNotifierItem *a, StatusNotifierItem *b);
};
} // namespace ns::systemtray
