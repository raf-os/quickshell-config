#include "weather.h"

#include <qhash.h>
#include <qhashfunctions.h>
#include <qjsonarray.h>
#include <qjsondocument.h>
#include <qjsonobject.h>
#include <qjsonparseerror.h>
#include <qlist.h>
#include <qlogging.h>
#include <qloggingcategory.h>
#include <qobject.h>
#include <qscopeguard.h>
#include <qtimer.h>
#include <qurl.h>
#include <qurlquery.h>

#include "config.h"
#include "helpermacros.h"
#include "modules.h"
#include "requests.h"

namespace ns::services {
Q_LOGGING_CATEGORY(logNSServicesWeather, "ns.services.weather")

AUTO_MEYERS_SINGLETON_QML_IMPL(WeatherService)

namespace {
constexpr int REFETCH_INTERVAL = 60 * 60 * 1000; // 1 hour

using WT = WeatherService::WeatherTypes;
const QHash<int, WT> TYPE_TABLE{
    {0,  WT::Clear       },
    {1,  WT::Clear       },
    {2,  WT::PartlyCloudy},
    {3,  WT::Cloudy      },
    {45, WT::Foggy       },
    {48, WT::Foggy       },
    {51, WT::Foggy       },
    {53, WT::Foggy       },
    {55, WT::Foggy       },
    {56, WT::Foggy       },
    {57, WT::Foggy       },
    {61, WT::Foggy       },
    {63, WT::Foggy       },
    {65, WT::Foggy       },
    {66, WT::Foggy       },
    {67, WT::Foggy       },
    {71, WT::Snowy       },
    {73, WT::Snowy       },
    {75, WT::Snowy       },
    {77, WT::Snowy       },
    {80, WT::Rainy       },
    {81, WT::Rainy       },
    {82, WT::Rainy       },
    {85, WT::Snowy       },
    {86, WT::Snowy       },
    {95, WT::Thunderstorm},
    {96, WT::Hail        },
    {97, WT::Thunderstorm},
    {99, WT::Hail        },
};
using WI = WeatherService::WeatherIntensity;
const QList<QPair<QList<int>, WI>> INTENSITY_TABLE = {
    {{0, 1, 2, 3, 45, 48, 51, 53, 55, 61, 66, 71, 77, 80, 85}, WI::Low   },
    {{53, 55, 56, 57, 63, 73, 81},                             WI::Medium},
    {{65, 67, 75, 82, 86, 95, 97},                             WI::High  }
};

template <typename T> T jsonArraySum(const QJsonArray &arr);

template <> qreal jsonArraySum(const QJsonArray &arr) {
  qreal sum = 0;
  for (auto val : arr) {
    if (!val.isDouble()) continue;
    sum += val.toDouble();
  }
  return sum;
}

template <> int jsonArraySum(const QJsonArray &arr) {
  int sum = 0;
  for (auto val : arr) {
    if (!val.isDouble()) continue;
    sum += val.toInt();
  }
  return sum;
}
} // namespace

WeatherService::WeatherService(QObject *parent) : QObject(parent) {
  m_refetchTimer.setInterval(REFETCH_INTERVAL);

  const auto weatherConfig = configs::Config::instance()->modules()->weather();
  b_latitude.setBinding(
      [weatherConfig] { return weatherConfig->bindableLatitude().value(); });
  b_longitude.setBinding(
      [weatherConfig] { return weatherConfig->bindableLongitude().value(); });

  b_weatherType.setBinding(
      [this] { return WeatherService::mapWeatherType(b_weatherCode.value()); });
  b_weatherIntensity.setBinding([this] {
    return WeatherService::mapWeatherIntensity(b_weatherCode.value());
  });

  QObject::connect(&m_refetchTimer, &QTimer::timeout, this,
      &WeatherService::onRefetchTimerTriggered);
  QObject::connect(configs::Config::instance()->modules()->enabledModules(),
      &configs::EnabledModules::weatherChanged, this,
      &WeatherService::evalEnabled);

  QObject::connect(this, &WeatherService::latitudeChanged, this,
      &WeatherService::onGeolocationChanged);
  QObject::connect(this, &WeatherService::longitudeChanged, this,
      &WeatherService::onGeolocationChanged);

  evalEnabled();
  fetchData();
}

void WeatherService::onWeatherRequestSuccess(const QByteArray &data) {
  auto self          = WeatherService::instance();
  self->m_isFetching = false;

  auto scope = qScopeGuard([self] {
    if (self->m_queuedFetch) {
      self->m_queuedFetch = false;
      self->fetchData();
    }
  });

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
    self->b_weatherCode        = cdata.value("weather_code").toInt(0);
  }
  auto daily = jobj.value("daily");
  if (daily.isObject()) {
    auto ddata = d.toObject();

    auto maxArray = ddata.value("temperature_2m_max").toArray();
    auto minArray = ddata.value("temperature_2m_min").toArray();

    self->b_dailyMaxTemperature = jsonArraySum<qreal>(maxArray);
    self->b_dailyMinTemperature = jsonArraySum<qreal>(minArray);
  }
}

void WeatherService::fetchData() {
  if (!configs::Config::instance()
          ->modules()
          ->enabledModules()
          ->bindableWeather()
          .value())
    return;

  if (m_isFetching) {
    m_queuedFetch = true;
    return;
  }

  QUrl fetchUrl("https://api.open-meteo.com/v1/"
                "forecast?");

  QUrlQuery query{
      {"latitude", QString::number(b_latitude.value(), 10, 6)},
      {"longitude", QString::number(b_longitude.value(), 10, 6)},
      {"current", "temperature_2m,weather_code"},
      {"daily", "temperature_2m_max,temperature_2m_min"},
      {"forecast_days", "1"},
      {"timeformat", "unixtime"},
      {"timezone", "auto"},
  };

  m_isFetching = true;

  fetchUrl.setQuery(query);

  RequestManager::get(fetchUrl, &WeatherService::onWeatherRequestSuccess,
      [this](const QByteArray &data) {
        m_isFetching = false;
        qCDebug(logNSServicesWeather).noquote()
            << "Error fetching data from weather api:" << data;

        if (m_queuedFetch) {
          m_queuedFetch = false;
          fetchData();
        }
      });
}

void WeatherService::onRefetchTimerTriggered() { fetchData(); }

void WeatherService::evalEnabled() {
  const auto isEnabled = configs::Config::instance()
                             ->modules()
                             ->enabledModules()
                             ->bindableWeather()
                             .value();
  if (isEnabled) {
    m_refetchTimer.start();
  } else {
    m_refetchTimer.stop();
  }
}

WeatherService::WeatherTypes WeatherService::mapWeatherType(int code) {
  if (TYPE_TABLE.contains(code)) {
    return TYPE_TABLE.value(code);
  } else return WeatherService::WeatherTypes::Clear;
}

WeatherService::WeatherIntensity WeatherService::mapWeatherIntensity(int code) {
  auto intensity = WeatherIntensity::Low;
  for (const auto &pair : INTENSITY_TABLE) {
    if (pair.first.contains(code)) {
      intensity = pair.second;
      break;
    }
  }
  return intensity;
}

void WeatherService::onGeolocationChanged() {
  if (!configs::Config::instance()
          ->modules()
          ->enabledModules()
          ->bindableWeather()
          .value())
    return;

  m_refetchTimer.start();
  fetchData();
}
} // namespace ns::services
