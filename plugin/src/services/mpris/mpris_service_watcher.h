#pragma once

#include <qabstractitemmodel.h>
#include <qdbusservicewatcher.h>
#include <qhash.h>
#include <qhashfunctions.h>
#include <qlist.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qqmlintegration.h>
#include <qqmllist.h>
#include <qtclasshelpermacros.h>
#include <qtmetamacros.h>
#include <qtypes.h>
#include <qvariant.h>

namespace ns::mpris {
class MprisPlayer;

class MprisPlayerListModel : public QAbstractListModel {
  Q_OBJECT
  QML_ELEMENT
  QML_UNCREATABLE("")

  Q_PROPERTY(QQmlListProperty<ns::mpris::MprisPlayer> values READ values NOTIFY
          valuesChanged)

public:
  explicit MprisPlayerListModel(QObject *parent = nullptr);

  enum Roles {
    ModelData = Qt::UserRole,
  };

  [[nodiscard]] qsizetype indexOf(MprisPlayer *player) const;

  void insert(MprisPlayer *player);
  bool removeOne(MprisPlayer *player);

  QHash<int, QByteArray> roleNames() const override;
  int                    rowCount(const QModelIndex &parent) const override;
  QVariant data(const QModelIndex &index, int role) const override;

  [[nodiscard]] QQmlListProperty<MprisPlayer> values();

  static qsizetype    qListCount(QQmlListProperty<MprisPlayer> *property);
  static MprisPlayer *qListAt(
      QQmlListProperty<MprisPlayer> *property, qsizetype index);

signals:
  void valuesChanged();

private:
  QList<MprisPlayer *> m_players;
};

class MprisServiceWatcher : public QObject {
  Q_OBJECT

public:
  explicit MprisServiceWatcher(QObject *parent = nullptr);
  Q_DISABLE_COPY_MOVE(MprisServiceWatcher)

  [[nodiscard]] bool                  isValid() const;
  [[nodiscard]] MprisPlayerListModel *model();

private slots:
  void onServiceRegistered(const QString &service);
  void onServiceUnregistered(const QString &service);
  void onPlayerDestroyed();
  void onPlayerReady();

private:
  bool                          m_isValid = false;
  QDBusServiceWatcher           m_watcher;
  QHash<QString, MprisPlayer *> m_players;
  MprisPlayerListModel         *m_readyPlayersModel;

  void registerPlayer(const QString &address);
};
} // namespace ns::mpris
