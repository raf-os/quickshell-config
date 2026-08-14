#pragma once

#include <qhash.h>
#include <qlist.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qpointer.h>
#include <qproperty.h>
#include <qqmlintegration.h>
#include <qqmllist.h>
#include <qstringview.h>
#include <qtmetamacros.h>
#include <qtypes.h>
#include <qvariant.h>

#include "toplevelhandle.h"
#include "wl_toplevel_handle.h"

namespace ns::hyprland {
class ToplevelInstance : public QObject {
  Q_OBJECT
  QML_ELEMENT
  QML_UNCREATABLE("")

  Q_PROPERTY(QString appId READ appId NOTIFY appIdChanged)
  Q_PROPERTY(QString title READ title NOTIFY titleChanged)
  Q_PROPERTY(quint64 address READ address NOTIFY addressChanged)
  Q_PROPERTY(int workspaceId READ workspaceId NOTIFY workspaceIdChanged)
  Q_PROPERTY(wayland::toplevels::WLToplevelHandle *waylandHandle READ
                 waylandHandle NOTIFY waylandHandleChanged)

public:
  explicit ToplevelInstance(wayland::wlr::toplevels::ToplevelHandle *handle,
                            QObject *parent = nullptr);
  explicit ToplevelInstance(quint64  address,
                            QObject *parent = nullptr);

  [[nodiscard]] QString appId() const;
  [[nodiscard]] QString title() const;
  [[nodiscard]] quint64 address() const;
  void                  setAddress(const quint64 &address);
  [[nodiscard]] wayland::wlr::toplevels::ToplevelHandle *handle() const;
  [[nodiscard]] wayland::toplevels::WLToplevelHandle    *waylandHandle();

  [[nodiscard]] int workspaceId() const;
  void              setWorkspaceId(int value);

  Q_INVOKABLE void activate();

  bool isValid() const;

public slots:
  void onToplevelMap(wayland::wlr::toplevels::ToplevelHandle *handle);

private slots:
  void onHyprAddress(wayland::wlr::toplevels::ToplevelHandle *handle,
                     quint64                                  address);

signals:
  void ready();
  void appIdChanged();
  void titleChanged();
  void addressChanged();
  void waylandHandleChanged();
  void workspaceIdChanged();

private:
  wayland::wlr::toplevels::ToplevelHandle *m_wlrHandle = nullptr;
  quint64                                  m_address;
  int                                      m_workspaceId = -1;
  bool                                     m_isValid     = false;

  void setupToplevelConnections();
};

class ToplevelModel : public QObject {
  Q_OBJECT
  QML_ELEMENT
  QML_UNCREATABLE("")

  Q_PROPERTY(QString searchQuery READ searchQuery WRITE setSearchQuery NOTIFY
                 searchQueryChanged)
  Q_PROPERTY(QQmlListProperty<ns::hyprland::ToplevelInstance> items READ items
                 NOTIFY itemsChanged)

public:
  explicit ToplevelModel(QObject *parent = nullptr);

  [[nodiscard]] QString searchQuery() const;
  void                  setSearchQuery(const QString &value);

  [[nodiscard]] QQmlListProperty<ToplevelInstance> items();
  [[nodiscard]] QList<ToplevelInstance *>          toplevelList() const;

signals:
  void searchQueryChanged();
  void itemsChanged();
  void readyToplevelsChanged(const QList<ToplevelInstance *> &newToplevels);
  void windowMoved(ToplevelInstance *toplevel);

public slots:
  void
  onWaylandToplevelCreated(wayland::wlr::toplevels::ToplevelHandle *toplevel);
  void
  onWaylandToplevelDestroyed(wayland::wlr::toplevels::ToplevelHandle *toplevel);
  void onAddressActivated(quint64 address);
  void handleHyprClientsPayload(const QByteArray &data);
  void onWindowMoveWorkspace(const quint64 &address,
                             int            workspaceId);

private:
  QList<ToplevelInstance *> m_allTopLevels;
  QList<ToplevelInstance *> m_readyToplevels;
  QList<ToplevelInstance *> m_filteredToplevels;

  QString m_searchQuery;

  ToplevelInstance *createNewInstance(const quint64 &address);
  ToplevelInstance *
  createNewInstance(wayland::wlr::toplevels::ToplevelHandle *handle);

  void insertAtEnd(ToplevelInstance *instance);
  void removeAtIndex(int index);
  void removeInstance(ToplevelInstance *instance);

  void applyFilters(QList<ToplevelInstance *> *target);
};
} // namespace ns::hyprland
