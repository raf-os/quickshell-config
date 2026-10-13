import qs.components

import org.nightshell.Configs
import org.nightshell.Utils

import QtQuick
import QtQuick.Layouts

MouseArea {
  id: root

  required property bool isActive

  readonly property int desiredWidth: 420

  property alias titleTransform: titleTransform
  property alias layoutTransform: layoutTransform
  property alias sideTransform: sideTransform

  implicitWidth: desiredWidth
  implicitHeight: layerWrapper.height

  signal exitAnimationFinished

  onIsActiveChanged: {
    if (!isActive)
      root.exitAnimationFinished();
  }

  Background {
    anchors {
      top: layerWrapper.top
      left: layerWrapper.left
    }
    width: layerWrapper.implicitWidth
    height: layerWrapper.implicitHeight

    contentItem: root
    fillItem: layerWrapper
  }

  Item {
    id: layerWrapper

    implicitWidth: root.desiredWidth + sideButtonsComponent.width
    implicitHeight: titleWrapper.implicitHeight + mainLayout.height

    layer.enabled: true
    visible: false

    anchors {
      top: parent.top
      left: parent.left
      topMargin: Styles.spacing_md
      leftMargin: Styles.spacing_md * 2
    }

    Rectangle {
      anchors.fill: mainLayout
      color: Colors.primaryContent
    }

    Item {
      id: titleWrapper

      readonly property int padding: Styles.padding_sm

      anchors {
        left: mainLayout.left
        right: mainLayout.right
      }

      implicitHeight: titleText.height

      Rectangle {
        anchors.fill: parent
        color: Colors.primary
      }

      StyledText {
        id: titleText

        AbsoluteTransform {
          id: titleTransform
          targetAncestor: layerWrapper
        }

        anchors {
          left: parent.left
          top: parent.top
        }

        padding: titleWrapper.padding

        text: "DASHBOARD"
        color: Colors.primaryContent
        font.pointSize: Styles.text_md
        font.weight: 600
      }
    }

    ColumnLayout {
      id: mainLayout

      anchors {
        top: titleWrapper.bottom
        left: parent.left
      }

      AbsoluteTransform {
        id: layoutTransform
        targetAncestor: layerWrapper
        padding: Qt.vector2d(mainLayout.anchors.leftMargin * 2, mainLayout.anchors.topMargin * 2)
      }

      width: root.desiredWidth
      spacing: Styles.spacing_md

      UserInfo {
        id: userInfoComponent
        readonly property int padding: Styles.padding_md
        Layout.margins: padding
      }

      WeatherComponent {
        id: weatherComponent
      }
    }

    SideButtons {
      id: sideButtonsComponent

      AbsoluteTransform {
        id: sideTransform
        targetAncestor: layerWrapper
      }

      anchors {
        top: titleWrapper.bottom
        left: mainLayout.right
      }
    }
  }
}
