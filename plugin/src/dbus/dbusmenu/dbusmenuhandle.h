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
  Q_PROPERTY(ns::dbusmenu::DBusMenuItem *item READ menu NOTIFY menuChanged)

public:
  explicit DBusMenuHandle(QObject *parent) : QObject(parent) {}

  void          setAddress(const QString &service, const QString &path);
  bool          isValid() const;
  DBusMenuItem *menu();

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
};

// A QML helper that implements a scoped ref counted mechanism
class DBusMenuOpener : public QObject {
  Q_OBJECT
  QML_ELEMENT
public:
  explicit DBusMenuOpener(QObject *parent = nullptr) : QObject(parent) {}
  ~DBusMenuOpener() override;
  Q_DISABLE_COPY_MOVE(DBusMenuOpener)

  [[nodiscard]] DBusMenuHandle *menu() const;
  void                          setMenu(DBusMenuHandle *menu);

signals:
  void menuChanged();

private slots:
  void onMenuDestroyed();

private:
  DBusMenuHandle *m_menu = nullptr;
};
} // namespace ns::dbusmenu
