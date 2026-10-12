#pragma once

#include <functional>

#include <qhashfunctions.h>
#include <qnetworkaccessmanager.h>
#include <qobject.h>
#include <qqmlengine.h>
#include <qtmetamacros.h>
#include <qurl.h>

#include "helpermacros.h"

namespace ns::services {
class RequestManager : public QObject {
  Q_OBJECT
  AUTO_MEYERS_SINGLETON_DECL(RequestManager)

public:
  [[nodiscard]] QNetworkAccessManager *manager();

  static void get(const QUrl &url, std::function<void(QByteArray)> onSuccess,
      std::function<void(QByteArray)> onError = nullptr);

private:
  explicit RequestManager(QObject *parent = nullptr);

  QNetworkAccessManager *m_manager = nullptr;
};
} // namespace ns::services
