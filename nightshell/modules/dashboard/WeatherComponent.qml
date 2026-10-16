import qs.components
import qs.components.weather

import org.nightshell.Services
import org.nightshell.Configs

import QtQuick
import QtQuick.Layouts

Loader {
  id: root
  active: Config.modules.enabledModules.weather

  Layout.fillWidth: true
  Layout.rightMargin: Styles.padding_lg

  sourceComponent: Item {
    id: weatherWidget

    implicitHeight: mainLayout.implicitHeight

    RowLayout {
      id: mainLayout

      anchors {
        top: parent.top
        left: parent.left
        right: parent.right
      }

      WeatherIcon {
        id: weatherIcon

        Layout.alignment: Qt.AlignVCenter
        Layout.margins: Styles.padding_lg

        weatherCode: WeatherService.weatherType
        size: 64

        color: Colors.primary
      }

      RowLayout {
        Layout.fillWidth: true
        Layout.fillHeight: true

        StyledText {
          Layout.fillWidth: true
          text: `${WeatherService.currentTemperature} °C`

          font.pointSize: Styles.text_xl * 1.25
          font.weight: 600
          color: Colors.primary
        }

        ColumnLayout {
          Layout.fillHeight: true

          SInfoLabelPair {
            label: "Minimum"
            text: `${WeatherService.dailyMinTemperature} °C`
          }

          SInfoLabelPair {
            label: "Maximum"
            text: `${WeatherService.dailyMaxTemperature} °C`
          }
        }
      }
    }
  }

  component SInfoLabelPair: InfoLabelPair {
    horizontalAlignment: Text.AlignRight
    text_color: Colors.primary
    label_color: Colors.primaryMuted
  }
}
