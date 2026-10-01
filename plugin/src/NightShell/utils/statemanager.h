#pragma once

#include <optional>

#include <qcontainerfwd.h>
#include <qjsengine.h>
#include <qloggingcategory.h>
#include <qobject.h>
#include <qqmlengine.h>
#include <qqmlintegration.h>
#include <qtimer.h>
#include <qtmetamacros.h>

#include "helpermacros.h"
#include "ns_utils_shared_global.h"

namespace ns::utils {
Q_DECLARE_LOGGING_CATEGORY(logNSUtilsStateManager)

struct UserStateData {
  QStringList favoriteApps;
};

class NS_UTILS_EXPORT StateManager : public QObject {
  Q_OBJECT
  QML_ELEMENT
  QML_SINGLETON

  AUTO_MEYERS_SINGLETON_QML_DECL(StateManager)

  Q_PROPERTY(
      QStringList favoriteApps READ favoriteApps NOTIFY favoriteAppsChanged)

public:
  [[nodiscard]] QStringList favoriteApps() const;

  Q_INVOKABLE void addFavoriteApp(const QString &appId);
  Q_INVOKABLE void removeFavoriteApp(const QString &appId);

signals:
  void favoriteAppsChanged();

private slots:
  void onStatePathChanged();
  void onSaveTimerTimeout();

private:
  explicit StateManager(QObject *parent = nullptr);

  std::optional<UserStateData> readUserStateData(const QString &path);
  void                         writeUserStateData(const QString &path);

  void queueSave();

  QTimer m_saveTimer;

  QStringList m_favoriteApps;
};
} // namespace ns::utils
