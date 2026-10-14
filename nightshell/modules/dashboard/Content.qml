import qs.components

import org.nightshell.Configs
import org.nightshell.Utils

import QtQuick
import QtQuick.Layouts

MouseArea {
  id: root

  required property bool isActive

  readonly property int desiredWidth: 420
  readonly property int resizeAnimationLength: 500
  readonly property int fadeAnimationLength: 1000

  property alias sessionTransform: sessionTransform

  implicitWidth: desiredWidth
  implicitHeight: layerWrapper.height

  signal exitAnimationFinished

  onIsActiveChanged: {
    if (isActive) {
      exitAnimation.stop();
      enterAnimation.restart();
    } else {
      enterAnimation.stop();
      exitAnimation.restart();
    }
  }

  ParallelAnimation {
    id: enterAnimation

    NAnim {
      target: layerWrapper
      property: "implicitWidth"
      to: root.desiredWidth
      duration: root.resizeAnimationLength
    }
    NumberAnimation {
      target: bg
      property: "opacity"
      to: 1
      duration: root.resizeAnimationLength * 0.5
    }
    NumberAnimation {
      target: layerWrapper
      property: "opacity"
      from: 0
      to: 1
      duration: root.fadeAnimationLength
      easing.type: Easing.InOutExpo
    }
  }

  SequentialAnimation {
    id: exitAnimation

    ParallelAnimation {
      NAnim {
        target: layerWrapper
        property: "implicitWidth"
        to: 0
        duration: root.resizeAnimationLength
      }
      NumberAnimation {
        target: bg
        property: "opacity"
        to: 0
        duration: root.resizeAnimationLength
        easing.type: Easing.InExpo
      }
      NumberAnimation {
        target: layerWrapper
        property: "opacity"
        to: 0
        duration: root.resizeAnimationLength
        easing.type: Easing.OutExpo
      }
    }
    ScriptAction {
      script: root.exitAnimationFinished()
    }
  }

  Background {
    id: bg
    anchors {
      top: layerWrapper.top
      left: layerWrapper.left
      // Offsetting by 0.5 pixels for a sharp outline when using the curve renderer
      topMargin: shouldOffset ? 0.5 : 0
      leftMargin: shouldOffset ? 0.5 : 0
    }
    width: layerWrapper.implicitWidth
    height: layerWrapper.implicitHeight
    opacity: 0

    contentItem: root
    fillItem: layerWrapper
  }

  Item {
    id: layerWrapper

    // implicitWidth: root.desiredWidth
    implicitHeight: mainLayout.height

    opacity: 0

    anchors {
      top: parent.top
      left: parent.left
      topMargin: Styles.spacing_md
      leftMargin: Styles.spacing_md * 4
    }
    clip: true

    ColumnLayout {
      id: mainLayout

      width: root.desiredWidth
      spacing: 0

      anchors {
        top: parent.top
        left: parent.left
      }

      Item {
        id: titleWrapper

        readonly property int paddingH: Styles.padding_lg
        readonly property int paddingV: Styles.padding_md

        Layout.fillWidth: true

        implicitHeight: titleText.height

        StyledText {
          id: titleText

          anchors {
            left: parent.left
            top: parent.top
          }

          text: "DASHBOARD"
          color: Colors.primary
          font.pointSize: Styles.text_lg
          font.weight: 600

          topPadding: titleWrapper.paddingV
          bottomPadding: topPadding
          leftPadding: titleWrapper.paddingH
          rightPadding: leftPadding
        }
      }

      CSeparator {}

      UserInfo {
        id: userInfoComponent
        readonly property int padding: Styles.padding_md
        Layout.margins: padding
      }

      WeatherComponent {
        id: weatherComponent
      }

      SessionComponent {
        id: sessionComponent

        AbsoluteTransform {
          id: sessionTransform
          targetAncestor: layerWrapper
        }
      }
    }
  }

  component CSeparator: Rectangle {
    id: cSeparator

    readonly property int margin: 8

    Layout.fillWidth: true
    Layout.leftMargin: margin
    Layout.rightMargin: margin
    implicitHeight: 1

    color: Colors.primaryMuted
  }
}
