import qs.components.weather

import org.nightshell.Services
import org.nightshell.Configs

import QtQuick
import QtQuick.Layouts

Loader {
  id: root
  active: Config.modules.enabledModules.weather

  Layout.fillWidth: true

  sourceComponent: Item {
    id: weatherWidget

    implicitHeight: mainLayout.implicitHeight

    RowLayout {
      id: mainLayout

      anchors {
        top: parent.top
      }

      WeatherIcon {
        id: weatherIcon

        Layout.alignment: Qt.AlignVCenter
        Layout.margins: Styles.padding_lg

        weatherCode: WeatherService.weatherType
        size: 64

        color: Colors.primary
      }

      ColumnLayout {
        Layout.fillWidth: true
        Layout.fillHeight: true

        InfoLabelPair {
          label_color: Colors.primary
          text_color: Colors.primary
          label: "Current temperature"
          text: `${WeatherService.currentTemperature} °C`
        }
      }
    }
  }
}
