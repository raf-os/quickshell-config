#pragma once

#include <cstdint>

#include <qhashfunctions.h>
#include <qlist.h>
#include <qobject.h>
#include <qproperty.h>
#include <qqmlintegration.h>
#include <qqmllist.h>
#include <qtimer.h>
#include <qtmetamacros.h>
#include <qtypes.h>

#include "helpermacros.h"

namespace ns::services {
class WeatherService : public QObject {
  Q_OBJECT
  QML_ELEMENT
  QML_SINGLETON

  AUTO_MEYERS_SINGLETON_QML_DECL(WeatherService)

public:
  enum WeatherTypes {
    Clear,
    Cloudy,
    PartlyCloudy,
    Foggy,
    Rainy,
    Hail,
    Thunderstorm,
    Snowy
  };
  Q_ENUM(WeatherTypes)

  enum WeatherIntensity : uint8_t { Low, Medium, High };
  Q_ENUM(WeatherIntensity)

#define B(Type, Name) AUTO_BINDABLE(WeatherService, Type, Name)
  B(qreal, latitude)
  B(qreal, longitude)
  B(qreal, currentTemperature)
  B(qreal, dailyMaxTemperature)
  B(qreal, dailyMinTemperature)
  B(ns::services::WeatherService::WeatherTypes, weatherType)
  B(ns::services::WeatherService::WeatherIntensity, weatherIntensity)
#undef B

  static WeatherTypes     mapWeatherType(int code);
  static WeatherIntensity mapWeatherIntensity(int code);

private slots:
  void fetchData();
  void onRefetchTimerTriggered();
  void evalEnabled();
  void onGeolocationChanged();

private:
  explicit WeatherService(QObject *parent = nullptr);

  static void onWeatherRequestSuccess(const QByteArray &data);

  bool   m_isFetching  = false;
  bool   m_queuedFetch = false;
  QTimer m_refetchTimer;

  Q_OBJECT_BINDABLE_PROPERTY(WeatherService, int, b_weatherCode)
};
} // namespace ns::services
