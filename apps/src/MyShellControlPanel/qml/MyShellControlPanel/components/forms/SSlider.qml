pragma ComponentBehavior: Bound

import MyShellPlugin
import MyShellPlugin.Configs
import QtQuick
import QtQuick.Effects
import QtQuick.Controls

Slider {
    id: root

    snapMode: Slider.SnapOnRelease

    background: Item {
        id: sliderBg

        Rectangle {
            id: filledBg

            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.left: parent.left

            anchors.topMargin: root.implicitHeight / 3
            anchors.bottomMargin: root.implicitHeight / 3

            implicitWidth: root.handle.x - root.implicitHeight / 6

            radius: height / 2
            topRightRadius: root.implicitHeight / 15
            bottomRightRadius: root.implicitHeight / 15

            color: Colors.colors.primary
        }

        Rectangle {
            id: remainingBg

            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.right: parent.right

            anchors.topMargin: root.implicitHeight / 3
            anchors.bottomMargin: root.implicitHeight / 3

            implicitWidth: parent.width - root.handle.x - root.handle.implicitWidth - root.implicitHeight / 6

            radius: height / 2
            topLeftRadius: root.implicitHeight / 15
            bottomLeftRadius: root.implicitHeight / 15

            color: Colors.colors.base
        }
    }

    handle: Item {
        z: 2

        x: root.leftPadding + root.visualPosition * (root.availableWidth - width)
        y: root.topPadding + root.availableHeight / 2 - height / 2

        implicitWidth: Config.appearance.fontSize.xxl
        implicitHeight: implicitWidth

        RectangularShadow {
            anchors.fill: parent

            radius: width / 2

            color: Colors.colors.neutral
            offset: Qt.vector2d(0, 2)
            blur: 8
            spread: -1
        }

        Rectangle {
            anchors.fill: parent

            color: Colors.colors.neutralContent

            radius: width / 2
        }

        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.NoButton
            cursorShape: Qt.PointingHandCursor
        }
    }
}
