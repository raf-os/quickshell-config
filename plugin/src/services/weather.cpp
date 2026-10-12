#include "weather.h"

#include <qhashfunctions.h>
#include <qjsondocument.h>
#include <qjsonobject.h>
#include <qjsonparseerror.h>
#include <qlogging.h>
#include <qloggingcategory.h>
#include <qobject.h>
#include <qtimer.h>
#include <qurl.h>
#include <qurlquery.h>

#include "helpermacros.h"
#include "requests.h"

namespace ns::services {
Q_LOGGING_CATEGORY(logNSServicesWeather, "ns.services.weather")

AUTO_MEYERS_SINGLETON_QML_IMPL(WeatherService)

namespace {
constexpr int REFETCH_INTERVAL = 60 * 60 * 1000; // 1 hour
}

WeatherService::WeatherService(QObject *parent) : QObject(parent) {
  m_refetchTimer.setInterval(REFETCH_INTERVAL);
  QObject::connect(&m_refetchTimer, &QTimer::timeout, this,
      &WeatherService::onRefetchTimerTriggered);
  fetchData(); //
}

void WeatherService::onWeatherRequestSuccess(const QByteArray &data) {
  auto self          = WeatherService::instance();
  self->m_isFetching = false;

  QJsonParseError error;
  auto            jdoc = QJsonDocument::fromJson(data, &error);

  if (error.error != QJsonParseError::NoError) {
    qCWarning(logNSServicesWeather)
        << "Error parsing weather json data from api response."
        << error.errorString();
    return;
  }
  if (!jdoc.isObject()) {
    qCWarning(logNSServicesWeather)
        << "Unexpected json data received - cannot parse.";
    return;
  }

  auto jobj = jdoc.object();
  auto d    = jobj.value("current");
  if (d.isObject()) {
    auto cdata = d.toObject();

    self->b_currentTemperature = cdata.value("temperature_2m").toDouble();
    self->b_weatherCode        = cdata.value("weather_code").toInt(-1);
  }
}

void WeatherService::fetchData() {
  if (m_isFetching) return;

  QUrl fetchUrl("https://api.open-meteo.com/v1/"
                "forecast?");

  QUrlQuery query{
      {"latitude", QString::number(b_latitude.value(), 10, 6)},
      {"longitude", QString::number(b_longitude.value(), 10, 6)},
      {"current", "temperature_2m,weather_code"},
      {"forecast_days", "1"},
      {"timeformat", "unixtime"}
  };

  m_isFetching = true;

  fetchUrl.setQuery(query);

  RequestManager::get(fetchUrl, &WeatherService::onWeatherRequestSuccess,
      [this](const QByteArray &data) {
        m_isFetching = false;
        qDebug() << "Error:\n" << data;
      });
}

void WeatherService::onRefetchTimerTriggered() { fetchData(); }
} // namespace ns::services
