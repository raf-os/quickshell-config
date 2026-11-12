import QtQuick

NumberAnimation {
  id: root

  property: "opacity"
  from: 0
  to: 1

  easing.type: Easing.InElastic
  easing.amplitude: 4
  easing.period: 0.4
}
