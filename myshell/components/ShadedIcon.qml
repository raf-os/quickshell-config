pragma ComponentBehavior: Bound

import Quickshell.Widgets
import QtQuick
import QtQuick.Effects

IconImage {
    id: root

    required property color iconColor

    asynchronous: true

    layer.enabled: true
    layer.effect: MultiEffect {
        saturation: -1.0
    }
}
