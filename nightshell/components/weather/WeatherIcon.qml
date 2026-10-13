pragma ComponentBehavior: Bound

import org.nightshell.Configs
import org.nightshell.Services
import org.nightshell.Utils

import QtQuick
import QtQuick.Effects
import QtQuick.VectorImage

Item {
  id: root

  required property int weatherCode
  property int size: 64

  property color color: Colors.neutralContent

  readonly property string weatherIcon: {
    switch (weatherCode) {
    case WeatherService.WeatherTypes.Clear:
      return "weather_clear";
    case WeatherService.WeatherTypes.Cloudy:
      return "weather_cloudy";
    case WeatherService.WeatherTypes.PartlyCloudy:
      return "weather_partly_cloudy";
    case WeatherService.WeatherTypes.Foggy:
      return "weather_foggy";
    case WeatherService.WeatherTypes.Rainy:
      return "weather_rainy";
    case WeatherService.WeatherTypes.Hail:
      return "weather_hail";
    case WeatherService.WeatherTypes.Thunderstorm:
      return "weather_thunderstorm";
    case WeatherService.WeatherTypes.Snowy:
      return "weather_snowy";
    default:
      return "weather_clear";
    }
  }

  implicitWidth: size
  implicitHeight: size

  VectorImage {
    id: vectorImage
    anchors.centerIn: parent

    asynchronous: true
    fillMode: VectorImage.PreserveAspectFit
    retainWhileLoading: true
    source: `${Paths.iconPath}/${root.weatherIcon}.svg`
    preferredRendererType: VectorImage.CurveRenderer

    width: root.size
    height: root.size
    visible: false
  }

  MultiEffect {
    anchors.fill: vectorImage
    source: vectorImage
    colorization: 1
    colorizationColor: root.color
  }
}
