#pragma once

#include <qabstractitemmodel.h>
#include <qhash.h>
#include <qlist.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qpointer.h>
#include <qproperty.h>
#include <qqmlintegration.h>
#include <qstringview.h>
#include <qtmetamacros.h>
#include <qtypes.h>
#include <qvariant.h>

#include "toplevelhandle.h"

namespace ns::hyprland {
class ToplevelInstance : public QObject {
  Q_OBJECT
  QML_ELEMENT
  QML_UNCREATABLE("")

  Q_PROPERTY(QString appId READ appId NOTIFY appIdChanged)
  Q_PROPERTY(QString title READ title NOTIFY titleChanged)
  Q_PROPERTY(quint64 address READ address NOTIFY addressChanged)
  Q_PROPERTY(int workspaceId READ workspaceId NOTIFY workspaceIdChanged)

public:
  explicit ToplevelInstance(toplevels::ToplevelHandle *handle,
                            QObject                   *parent = nullptr);
  explicit ToplevelInstance(quint64  address,
                            QObject *parent = nullptr);

  [[nodiscard]] QString                    appId() const;
  [[nodiscard]] QString                    title() const;
  [[nodiscard]] quint64                    address() const;
  [[nodiscard]] toplevels::ToplevelHandle *handle() const;

  [[nodiscard]] int workspaceId() const;
  void              setWorkspaceId(int value);

  Q_INVOKABLE void activate();

  bool isValid() const;

public slots:
  void onToplevelMap(toplevels::ToplevelHandle *handle);

private slots:
  void onHyprAddress(toplevels::ToplevelHandle *handle,
                     quint64                    address);

signals:
  void ready();
  void appIdChanged();
  void titleChanged();
  void addressChanged();
  void workspaceIdChanged();

private:
  toplevels::ToplevelHandle *m_waylandHandle = nullptr;
  quint64                    m_address;
  int                        m_workspaceId = -1;
  bool                       m_isValid     = false;

  void setupToplevelConnections();
};

class ToplevelModel : public QAbstractListModel {
  Q_OBJECT
  QML_ELEMENT

public:
  explicit ToplevelModel(QObject *parent = nullptr);

  enum Roles {
    ModelDataRole = Qt::UserRole + 1,
  };
  QHash<int,
        QByteArray>
           roleNames() const override;
  qint32   rowCount(const QModelIndex &parent = {}) const override;
  QVariant data(const QModelIndex &index,
                qint32             role) const override;

public slots:
  void onWaylandToplevelCreated(toplevels::ToplevelHandle *toplevel);
  void onWaylandToplevelDestroyed(toplevels::ToplevelHandle *toplevel);
  void onAddressActivated(quint64 address);
  void handleHyprClientsPayload(const QByteArray &data);

private:
  QList<ToplevelInstance *> m_allTopLevels;
  QList<ToplevelInstance *> m_readyToplevels;

  ToplevelInstance *createNewInstance(const quint64 &address);
  ToplevelInstance *createNewInstance(toplevels::ToplevelHandle *handle,
                                      bool noModelUpdate = false);

  void insertAtIndex(ToplevelInstance *instance,
                     int               index,
                     bool              noModelUpdate = false);
  void insertAtEnd(ToplevelInstance *instance,
                   bool              noModelUpdate = false);
  void removeAtIndex(int index);
  void removeInstance(ToplevelInstance *instance);
};
} // namespace ns::hyprland
