import QtQuick

NumberAnimation {
  id: root

  property: "opacity"
  from: 0
  to: 1

  easing.type: Easing.InElastic
  easing.amplitude: 0.2
  easing.period: 0.25
}
