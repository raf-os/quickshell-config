#include "requests.h"

#include <qhashfunctions.h>
#include <qlogging.h>
#include <qnetworkaccessmanager.h>
#include <qnetworkreply.h>
#include <qnetworkrequest.h>
#include <qobject.h>
#include <qqmlengine.h>
#include <qurl.h>

#include "helpermacros.h"

namespace ns::services {
AUTO_MEYERS_SINGLETON_IMPL(RequestManager)

RequestManager::RequestManager(QObject *parent)
    : QObject(parent), m_manager(new QNetworkAccessManager(this)) {}

QNetworkAccessManager *RequestManager::manager() { return m_manager; }

void RequestManager::get(const QUrl &url,
    std::function<void(QByteArray)>  onSuccess,
    std::function<void(QByteArray)>  onError) {
  auto self = RequestManager::instance();

  QNetworkRequest request(url);
  request.setAttribute(QNetworkRequest::CacheLoadControlAttribute,
      QNetworkRequest::AlwaysNetwork);
  request.setAttribute(
      QNetworkRequest::CookieSaveControlAttribute, QNetworkRequest::Manual);
  request.setRawHeader("Cache-Control", "no-cache, no-store");
  request.setRawHeader("Pragma", "no-cache");
  request.setRawHeader("Connection", "close");

  auto *reply = self->manager()->get(request);

  QObject::connect(reply, &QNetworkReply::finished, self,
      [self, reply, onSuccess, onError]() {
        const auto byteData = reply->readAll();

        if (reply->error() == QNetworkReply::NoError) {
          if (onSuccess) onSuccess(byteData);
        } else {
          if (onError) onError(byteData);
        }

        reply->deleteLater();
      });
}
} // namespace ns::services
