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

    Rectangle {
      id: separator
      anchors {
        top: parent.top
        left: parent.left
        right: parent.right
      }

      implicitHeight: 1
      color: Colors.primary
    }

    RowLayout {
      id: mainLayout

      anchors {
        top: separator.bottom
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
          label: "Current temperature"
          text: `${WeatherService.currentTemperature} °C`
        }
      }
    }
  }
}
