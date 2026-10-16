#include "geo_query.h"

#include <memory>
#include <utility>

#include <qhashfunctions.h>
#include <qjsonarray.h>
#include <qjsondocument.h>
#include <qjsonobject.h>
#include <qjsonparseerror.h>
#include <qlist.h>
#include <qloggingcategory.h>
#include <qobject.h>
#include <qqmlinfo.h>
#include <qqmllist.h>
#include <qscopeguard.h>
#include <qtypes.h>
#include <qurl.h>
#include <qurlquery.h>

#include "config.h"
#include "ns_services_debug.h"
#include "requests.h"

static constexpr int DEBOUNCE_TIMEOUT = 1000;

static inline void unexpected_data_error_log() {
  qCWarning(ns::services::NS_LOG_SERVICES())
      << "Received unexpected geolocation data from API - aborting.";
}

namespace ns::services {
GeolocationResult::GeolocationResult() {}
GeolocationResult::~GeolocationResult() = default;

GeolocationQuery::GeolocationQuery(QObject *parent) : QObject(parent) {
  const auto isEnabled = configs::Config::instance()
                             ->modules()
                             ->enabledModules()
                             ->bindableWeather()
                             .value();

  if (!isEnabled) {
    qmlWarning(this) << "Attempted using GeolocationQuery qml type, but "
                        "weather services are not enabled.";
    deleteLater();
    return;
  }

  m_results->reserve(10);

  m_debouceTimer.setSingleShot(true);
  m_debouceTimer.setInterval(DEBOUNCE_TIMEOUT);

  QObject::connect(&m_debouceTimer, &QTimer::timeout, this,
      &GeolocationQuery::onDebounceTimeout);
}
GeolocationQuery::~GeolocationQuery() = default;

// qsizetype GeolocationQuery::geo_list_count(RESULT_QML_LIST *property) {
//   return static_cast<RESULT_LIST *>(property->data)->size();
// }
// GeolocationResult *GeolocationQuery::geo_list_at(
//     RESULT_QML_LIST *property, qsizetype index) {
//   return static_cast<RESULT_LIST *>(property->data)->at(index);
// }
//
// QQmlListProperty<GeolocationResult> GeolocationQuery::results() {
//   return QQmlListProperty<GeolocationResult>(this, &m_results,
//       &GeolocationQuery::geo_list_count, &GeolocationQuery::geo_list_at);
// }

QList<GeolocationResult> *GeolocationQuery::results() const {
  return m_results.get();
}

QString GeolocationQuery::query() const { return m_query; }
void    GeolocationQuery::setQuery(const QString &value) {
  if (m_query == value) return;

  m_query = value;
  emit queryChanged();
  m_debouceTimer.start();
}

void GeolocationQuery::onDebounceTimeout() {
  if (b_isPending.value()) {
    m_queuedQuery = true;
    return;
  }

  b_isPending = true;

  const QString formattedQuery = m_query.replace(" ", "+");
  QUrl      fetchUrl{QString("https://geocoding-api.open-meteo.com/v1/search")};
  QUrlQuery fetchQuery{
      {"name",  formattedQuery},
      {"count", "10"          }
  };
  fetchUrl.setQuery(fetchQuery);

  RequestManager::get(
      fetchUrl,
      [this](const QByteArray &result) {
        b_isPending = false;

        auto scope = qScopeGuard([this] {
          if (m_queuedQuery) {
            m_queuedQuery = false;
            m_debouceTimer.start();
          }
        });

        QJsonParseError error;
        auto            jDoc = QJsonDocument::fromJson(result, &error);

        if (error.error != QJsonParseError::NoError) {
          qCWarning(NS_LOG_SERVICES)
              << "Error parsing geolocation json data." << error.errorString();
          return;
        }

        if (!jDoc.isObject()) {
          return unexpected_data_error_log();
        }

        auto jObj    = jDoc.object();
        auto results = jObj.value("results");
        if (results.isArray()) {
          return unexpected_data_error_log();
        }

        m_results->clear();

        for (auto item : results.toArray()) {
          if (!item.isObject()) continue;
          auto obj = item.toObject();

          auto name = obj.value("name").toString();
          if (name.isEmpty()) continue;

          GeolocationResult d;
          d.latitude     = obj.value("latitude").toDouble();
          d.longitude    = obj.value("longitude").toDouble();
          d.country      = obj.value("country").toString();
          d.country_code = obj.value("country_code").toString();
          d.timezone     = obj.value("timezone").toString();
          d.admin1       = obj.value("admin1").toString();

          m_results->append(std::move(d));
        }

        emit resultsChanged();
      },
      [this](const QByteArray &data) {
        b_isPending = false;
        qCWarning(NS_LOG_SERVICES).noquote()
            << "Error fetching geolocation data from API:" << data;
        if (m_queuedQuery) {
          m_queuedQuery = false;
          m_debouceTimer.start();
        }
      });
}
} // namespace ns::services
