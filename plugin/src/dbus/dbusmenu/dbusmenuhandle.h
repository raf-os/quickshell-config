#pragma once

#include <qcontainerfwd.h>
#include <qobject.h>
#include <qpointer.h>
#include <qqmlintegration.h>
#include <qtclasshelpermacros.h>
#include <qtmetamacros.h>
#include <qtypes.h>

// #include "dbusmenuitem.h"

namespace ns::dbusmenu {
class DBusMenu;
class DBusMenuItem;

class DBusMenuHandle : public QObject {
  Q_OBJECT
  QML_ELEMENT
  QML_UNCREATABLE("")

  Q_MOC_INCLUDE("dbusmenuitem.h")
  Q_PROPERTY(
      ns::dbusmenu::DBusMenuItem *rootItem READ rootMenu NOTIFY menuChanged)

public:
  explicit DBusMenuHandle(QObject *parent) : QObject(parent) {}

  void          setAddress(const QString &service, const QString &path);
  bool          isValid() const;
  DBusMenuItem *rootMenu();
  // can be nullptr
  DBusMenuItem *getItemById(qint32 id);

  void addRef();
  void removeRef();

signals:
  void menuChanged();

private slots:
  void onMenuPathChanged();

private:
  bool      m_isLoaded;
  QString   m_service;
  QString   m_path;
  DBusMenu *m_menu     = nullptr;
  quint32   m_refcount = 0;
  qint32    m_maxDepth = 1;
};

class DBusMenuOpener : public QObject {
  Q_OBJECT
  QML_ELEMENT

  Q_PROPERTY(ns::dbusmenu::DBusMenuHandle *menu READ menu WRITE setMenu NOTIFY
          menuChanged REQUIRED)
  Q_PROPERTY(ns::dbusmenu::DBusMenuItem *item READ item NOTIFY itemChanged)
  Q_PROPERTY(qint32 parent READ offset WRITE setOffset NOTIFY offsetChanged)

public:
  explicit DBusMenuOpener(QObject *parent = nullptr) : QObject(parent) {}
  ~DBusMenuOpener() override;
  Q_DISABLE_COPY_MOVE(DBusMenuOpener)

  [[nodiscard]] DBusMenuHandle *menu() const;
  void                          setMenu(DBusMenuHandle *menu);

  [[nodiscard]] DBusMenuItem *item();

  [[nodiscard]] qint32 offset() const;
  void                 setOffset(qint32 offset);

  void tryAttach();

signals:
  void menuChanged();
  void itemChanged();
  void offsetChanged();

private slots:
  void onMenuDestroyed();
  void onItemDestroyed();

private:
  bool            m_valid;
  qint32          m_watchedIndex;
  DBusMenuHandle *m_menu = nullptr;
  DBusMenuItem   *m_item = nullptr;
};
} // namespace ns::dbusmenu
