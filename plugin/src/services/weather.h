#pragma once

#include <qhashfunctions.h>
#include <qobject.h>
#include <qproperty.h>
#include <qqmlintegration.h>
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

#define B(Type, Name) AUTO_BINDABLE_WRITABLE(WeatherService, Type, Name)
  B(qreal, latitude)
  B(qreal, longitude)
#undef B

private slots:
  void fetchData();
  void onRefetchTimerTriggered();

private:
  explicit WeatherService(QObject *parent = nullptr);

  static void onWeatherRequestSuccess(const QByteArray &data);

  bool   m_isFetching = false;
  QTimer m_refetchTimer;

  Q_OBJECT_BINDABLE_PROPERTY(WeatherService, int, b_weatherCode)
  Q_OBJECT_BINDABLE_PROPERTY(WeatherService, qreal, b_currentTemperature)
};
} // namespace ns::services
