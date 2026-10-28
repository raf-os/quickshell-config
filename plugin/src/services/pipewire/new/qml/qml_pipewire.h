#pragma once

#include <memory>

#include <qabstractitemmodel.h>
#include <qhash.h>
#include <qhashfunctions.h>
#include <qlist.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qqmlintegration.h>
#include <qtmetamacros.h>
#include <qtypes.h>

#include "helpermacros.h"
#include "pw_defaults.h"
#include "pw_node.h"

namespace ns::pipewire {
class QMLPipewireGlobal : public QObject {
  Q_OBJECT
  QML_NAMED_ELEMENT(Pipewire)
  QML_SINGLETON

  AUTO_MEYERS_SINGLETON_QML_DECL(QMLPipewireGlobal)

#define PROP(Name)                                                             \
  Q_PROPERTY(                                                                  \
      Node *default##Name READ default##Name NOTIFY default##Name##Changed)    \
public:                                                                        \
  [[nodiscard]] Node *default##Name();                                         \
  Q_SIGNAL void default##Name##Changed();

  PROP(Sink)
  PROP(Source)
  PROP(ConfiguredSink)
  PROP(ConfiguredSource)
#undef PROP

private slots:
  void onDefaultNodeChanged(DefaultTracker::Type type);

private:
  explicit QMLPipewireGlobal(QObject *parent = nullptr);
};

class PipewireModel : public QAbstractItemModel {
  Q_OBJECT
  QML_NAMED_ELEMENT(PipewireModel)

  using device_list_t = QList<quint32>;

public:
  enum Roles {
    ModelDataRole   = Qt::UserRole + 1,
    NameRole        = Qt::UserRole + 2,
    DescriptionRole = Qt::UserRole + 3,
    NicknameRole    = Qt::UserRole + 4,
  };

  QHash<int, QByteArray> roleNames() const override;
  int                    rowCount(const QModelIndex &parent) const override;
  int                    columnCount(const QModelIndex &parent) const override;

  QModelIndex index(
      int row, int column, const QModelIndex &parent) const override;
  QModelIndex parent(const QModelIndex &index) const override;
  QVariant    data(const QModelIndex &index, int role) const override;

private slots:
  void onNodeAdded(TrackableNode *node);
  void onNodeDestroyed();

private:
  QHash<quint32, std::shared_ptr<Node>> m_trackedNodes;

  [[nodiscard]] QModelIndex listToIndex(device_list_t *, int row) const;

  device_list_t m_audioSinks;
  device_list_t m_audioSources;
};
} // namespace ns::pipewire
