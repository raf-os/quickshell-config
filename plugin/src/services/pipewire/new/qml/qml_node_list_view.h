#pragma once

#include <qabstractitemmodel.h>
#include <qqmlintegration.h>
#include <qtmetamacros.h>
namespace ns::pipewire {
class QMLNodeListView : public QAbstractListModel {
  Q_OBJECT
  QML_NAMED_ELEMENT(PwNodeListView)

  Q_PROPERTY(ns::pipewire::PipewireModel *model READ model WRITE setModel NOTIFY
          modelChanged)
  Q_PROPERTY(QModelIndex rootIndex READ rootIndex WRITE setRootIndex NOTIFY
          rootIndexChanged)
};
} // namespace ns::pipewire
